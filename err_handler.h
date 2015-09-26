#ifndef _ERR_HANDLER_H
#define _ERR_HANDLER_H
#include <errno.h>

#ifdef __GNUC__
#define likely(x)	__builtin_expect((x), 1)
#define unlikely(x)	__builtin_expect((x), 0)
#else
#define likely(x)	(x)
#define unlikely(x)	(x)
#endif

#define ERR_BUFFER	1024

#define err_debugon()	do { debug = 1; } while (0)
#define err_debugoff()	do { debug = 0; } while (0)
#define err_daemonon()	do { daemon = 1; } while (0)
#define err_daemonoff()	do { daemon = 0; } while (0)

typedef enum { false = 0, true = 1 } bool;

void err_init(void) __attribute__((constructor));
void err_close(void) __attribute__((destructor));

void err_dbg(const char *msg, ...) __attribute__((format(printf, 1, 2)));
void err_msg(const char *msg, ...) __attribute__((format(printf, 1, 2)));
void err_sys(const char *msg, ...) __attribute__((format(printf, 1, 2)));
void err_exit(const char *msg, ...) __attribute__((noreturn,
			format(printf, 1, 2)));
#endif
