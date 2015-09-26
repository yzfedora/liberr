#define _DEFAULT_SOURCE		/* vsyslog() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

#define call_err_internal(doexit, doerr, msg)		\
	do {						\
		va_list ap;				\
		va_start(ap, msg);			\
		err_internal(doexit, doerr, msg, ap);	\
		va_end(ap);				\		
	} while (0)

static int debug = 0;
static int daemon = 0;

static void err_internal(bool doexit, bool doerr, char *msg, va_list ap)
{
	/* Prevent errno be modified in a asynchronous signal handler. */
	int __errno = errno;
	size_t len;		/* For optimize times of call strlen() */
	char buf[ERR_BUFFER];
	
	vsnprintf(buf, sizeof(buf), msg, ap);
	if (likely(doerr)) {
		len = strlen(buf);
		strncat(buf + len, ": ", sizeof(buf) - len);
		len += 2;
		strerror_r(__errno, buf + len, sizeof(buf) - len);
	}
	strncat(buf, "\n", sizeof(buf) - strlen(buf));

	if (unlikely(daemon))
		syslog(LOG_DEBUG, buf);
	else
		fprintf(stderr, buf);
	if (doexit)
		exit(EXIT_FAILURE);
}

void err_dbg(const char *msg, ...)
{
	if (!debug)
		return;

	call_err_internal(false, false, msg);
}

void err_msg(const char *msg)
{
	call_err_internal(false, false, msg);
}

void err_sys(const char *msg)
{
	call_err_internal(false, true, msg);
}

void err_exit(const char *msg)
{
	call_err_internal(true, true, msg);
}

void err_init(void) __attribute__((constructor))
{
	printf("executing err_init\n");
}

void err_fini(void) __attribute__((destructor))
{
	printf("executing err_fini\n");
}
