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
 * This is a sub test program, it should be called by sigtstp_test directly.
 */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <err_handler.h>

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

int main(int argc, char *argv[])
{
	if (argc != 2) {
		err_msg("Usage: %s times", argv[0]);
		return 1;
	}

	char buf[32];
	int times = atoi(argv[1]);

	while (times--) {
		/* Fake a error used to test we optimized for err_sys() and
		 * err_exit(). */
		errno = random_errno_generator();
		err_sys(random_string_generator(buf, sizeof(buf)));
	}
	return 0;
}
