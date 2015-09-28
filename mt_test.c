
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
 * This is multi-thread test for liberr.
 */
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <err_handler.h>

#define	MT_TEST_OUT	"mt_test.out"
#define LITTER_RANGE	('z' - 'A')
#define ERRNO_MAX	132

static __attribute__((constructor)) void init(void)
{
	srandom(time(NULL));
}

static char *random_string_generator(char *buf, size_t len)
{
	int i = 0;
	long r;

	while (i < len - 1) {
		r = random() % (LITTER_RANGE + 1);
		buf[i++] = (char)('A' + r); 
	}
	return buf;
}

static inline int random_errno_generator(void)
{
	/* generator number 0 ~ (ERRNO_MAX - 1), but errno start at 1. */
	int r = 1;
	r += random() % ERRNO_MAX;
	return r;
}

static void *test_run(void *arg)
{
	char buf[32];
	int times = atoi(arg);

	while (times--) {
		/* Fake a error used to test we optimized for err_sys() and
		 * err_exit(). */
		errno = random_errno_generator();
		err_sys("%ld %s", (long)getpid(),
				random_string_generator(buf, sizeof(buf)));
	}
	return NULL;
}

static void mt_test_run(int num_threads, void *arg)
{
	int i;
	int s;
	pthread_t *thread;

	if (!(thread = calloc(num_threads, sizeof(*thread))))
		err_exit("calloc");

	for (i = 0; i < num_threads; i++) {
		if ((s = pthread_create(thread + i, NULL, test_run, arg)) != 0)
			err_exit("pthread_create");
	}

	for (i = 0; i < num_threads; i++) {
		if ((s = pthread_join(thread[i], NULL)) != 0)
			err_exit("pthread_join");
	}

	/* All launched threads has finished. */
}

static void redirect_stderr(void)
{
	int fd = open(MT_TEST_OUT, O_RDWR | O_CREAT | O_TRUNC, 0640);

	if (fd == -1)
		err_exit("open");

	err_setout(fd);

	if (close(fd) == -1)
		err_exit("close");
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s num-threads times\n", argv[0]);
		return 1;
	}

	redirect_stderr();
	mt_test_run(atoi(argv[1]), argv[2]);
	return 0;
}
