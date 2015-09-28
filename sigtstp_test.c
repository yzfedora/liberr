/* This is a general error process library for linux in C
 *   		Copyright (C) 2015  Yang Zhang <yzfedora@gmail.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/
/*
 * To figure out the result of test is correct or not, using the following way:
 *	In general, the error message's form:
 *		"\e[31m ABCDEFG: HIJKLMN\n\e[0m"
 *	This could be the interrupted form of when output error:
 *		"\e[31m ABCDEFG: HIJKLMN\nm"
 *	    or
 *		"\e[31m ABCDEFG: HIJ"--sigtstp--"KLMN\n\e[0m"
 *	So, the solution is appending a string "\e[0m" to the terminal when
 *	SIGTSTP is arrived, and output a string "\e[31m" when SIGCONT is
 *	arrived(before the remianning error output).
 *
 *	If test is ok, the flags string will output by liberr, it should be
 *	"#TSTP##CONT#", and no any characters in the middle between two "##".
 */
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err_handler.h>

#define	TEST_OUT	"test.out"
#define	TEST_TMP		"test.tmp"
#define	LINE		1024
static int times;
static int interval;

static __attribute__((constructor(255))) void init(void)
{
	srandom(time(NULL));
}

static void random_sleep(void)
{
	int t = random() % interval;	/* maximum is 500 millisecond */
	usleep(t);
}

static int test_result_analysis(void)
{
	FILE *fp = fopen(TEST_TMP, "r");
	char line[LINE];
	int counter = 0;

	if (!fp)
		err_exit("open %s for analysis", TEST_TMP);

	/* Find the Marker when liberr was stopped and continue by signal
	 * SIGTSTP or SIGCONT. */
	while (fgets(line, sizeof(line), fp)) {
		if (strstr(line, "#TSTP##CONT#"))
			counter++;
		else if (strstr(line, "#TSTP#") || strstr(line, "#CONT#"))
			printf("MISMATCH: %s", line);
	}

	if (!feof(fp))
		err_sys("read from the %s", TEST_TMP);
	if (fclose(fp) == EOF)
		err_sys("close file %s", TEST_TMP);

	return counter;
}

static void test_statistics_analysis(int *tstp, int *cont)
{
	FILE *fp = fopen(TEST_OUT, "r");
	char line[LINE];

	*tstp = 0;
	*cont = 0;
	if (!fp)
		err_sys("open statistics file %s", TEST_OUT);

	if (fgets(line, sizeof(line), fp)) {
		errno = 0;
		int ret = sscanf(line, "%d %d", tstp, cont);
		if (ret != 2 && errno)
			err_sys("extract test statistics");
	}

	if (!feof(fp))
		err_sys("read from the %s", TEST_OUT);
	if (fclose(fp) == EOF)
		err_sys("close file %s", TEST_OUT);
}

static void sigchld_handler(int signo)
{
	int tstp, cont, matched;

	test_statistics_analysis(&tstp, &cont);
	matched = test_result_analysis();
	printf("-------------------- liberr stop test -----------------\n");
	printf("Total tests %d times, matched %d times. (tstp %d, cont %d)\n",
			times, matched, tstp, cont);

	if (unlink(TEST_TMP) == -1)
		err_sys("Delete file %s", TEST_TMP);
	if (unlink(TEST_OUT) == -1)
		err_sys("Delete file %s", TEST_TMP);
	exit(0);
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		err_msg("Usage: %s times-to-test interval", argv[0]);
		return 1;
	}

	pid_t pid;
	struct sigaction act, oact;

	interval = atoi(argv[2]) * 1000;

	/* Because we are testing SIGTSTP and SIGCONT for liberr, we don't
	 * expect to received notify when child(liberr) stop or continue. */
	act.sa_handler = sigchld_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_NOCLDSTOP;

	if (sigaction(SIGCHLD, &act, &oact) == -1)
		err_exit("sigaction");

	fflush(NULL);
	if ((pid = fork()) == -1) {
		err_exit("fork");
	} else if (pid == 0) {
		int out;

#define	OPEN_FLAGS	(O_RDWR | O_CREAT | O_TRUNC)
		if ((out = open(TEST_TMP, OPEN_FLAGS, 0640)) == -1)
			err_exit("open");
		if (dup2(out, STDERR_FILENO) == -1)
			err_exit("Redirect stderr by dup2");
		if (close(out) == -1)
			err_sys("close");
		if (execlp("./pression_test", "./pression_test",
					argv[1], NULL) == -1)
			err_exit("execvp");
	}

	while (1) {
		random_sleep();
		kill(pid, SIGTSTP);
		random_sleep();
		kill(pid, SIGCONT);
		times++;
	}

	return 0;
}
