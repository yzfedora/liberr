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
#define _DEFAULT_SOURCE		/* vsyslog() */
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>
#include "err_handler.h"

#define ERR_BUFFER	1024
#define COLOR_RST	"\e[0m"		/* Reset color of terminal. */
#define COLOR_RED	"\e[31m"	/* Set red font. */

#define call_err_internal(doexit, doerr, level, msg)		\
	do {							\
		va_list ap;					\
		va_start(ap, msg);				\
		err_internal(doexit, doerr, level, msg, ap);	\
		va_end(ap);					\
	} while (0)

static int _err_debug = 0;
static int _err_daemon = 0;
static int _err_tty = 0;

void err_setdebug(bool flags)
{
	if (flags)	_err_debug = 1;
	else		_err_debug = 0;
}

void err_setdaemon(bool flags)
{
	if (flags)	_err_daemon = 1;
	else		_err_daemon = 0;
}

__attribute__((constructor(255))) void err_init(void)
{
	_err_tty = isatty(STDERR_FILENO);
	openlog(NULL, LOG_NDELAY | LOG_PID, LOG_USER);
}

__attribute__((destructor(255))) void err_fini(void)
{
	closelog();
	/* ERROR FOR USE sizeof(COLOR_RST) */
	if (write(STDERR_FILENO, COLOR_RST, sizeof(COLOR_RST) - 1) !=
			(sizeof(COLOR_RST) - 1))
		err_sys("liberr set color to default error");
}

static void err_internal(bool doexit, bool doerr, int level,
		const char *msg, va_list ap)
{
	/* Prevent errno be modified in a asynchronous signal handler. */
	int __errno = errno;
	size_t len = 0;		/* For optimize times of call strlen() */
	char buf[ERR_BUFFER];

	/* If print the errno, set the color of fonts to red. */
	if (likely(doerr && _err_tty)) {
		strncpy(buf, COLOR_RED, sizeof(buf));
		len += 5;
	}

	vsnprintf(buf + len, sizeof(buf) - len, msg, ap);
	
	/* Append ": " and error string to the end of 'buf'. */
	if (likely(doerr && __errno)) {
		len = strlen(buf);
		strncat(buf + len, ": ", sizeof(buf) - len);
		len += 2;
		strerror_r(__errno, buf + len, sizeof(buf) - len);
	}

	/* Append "\n" to the end of 'buf'. */
	len = strlen(buf);
	strncat(buf + len, "\n", sizeof(buf) - len);

	/* Set terminal mode to default. */
	if (likely(doerr && _err_tty)) {
		len += 1;
		strncat(buf + len, COLOR_RST, sizeof(buf) - len);
	}

	if (unlikely(_err_daemon))
		syslog(level, buf);
	else
		fprintf(stderr, buf);

	/* 'doexit' is true, used by err_exit(). */
	if (doexit)
		exit(EXIT_FAILURE);
}

void err_dbg(const char *msg, ...)
{
	if (likely(!_err_debug))
		return;

	call_err_internal(false, false, LOG_DEBUG, msg);
}

void err_msg(const char *msg, ...)
{
	call_err_internal(false, false, LOG_INFO, msg);
}

void err_sys(const char *msg, ...)
{
	call_err_internal(false, true, LOG_ERR, msg);
}

void err_exit(const char *msg, ...)
{
	call_err_internal(true, true, LOG_ERR, msg);
	/* Force to tell compiler this is 'noreturn', because of in the macro
	 * 'call_err_internal', the first argument 'doexit' is true. */
	__builtin_unreachable();
}
