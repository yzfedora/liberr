# Description
	This is a general library for the programs to print error or messages.

# Functions
	+-------------------------------+------------+----------+-----------+
	|       prototypes              | strerror() | behavior | loglevel  |
	+-------------------------------+------------+----------+-----------+
	| err_dbg(const char *msg, ...) |    NO      |  return  | SYS_DEBUG |
	+-------------------------------+------------+----------+-----------+
	| err_msg(const char *msg, ...) |    NO      |  return  |  SYS_INFO |
	+-------------------------------+------------+----------+-----------+
	| err_sys(const char *msg, ...) |    YES     |  return  |  SYS_ERR  |
	+-------------------------------+------------+----------+-----------+
	| err_exit(const char *msg, ...)|    YES     |   exit   |  SYS_ERR  |
	+-------------------------------+------------+----------+-----------+

# Notice
	When you're using the some functions like 'pthread_create', I
	recommend you using following statements:

		int err;
		pthread_t thread;

		if ((err = pthread_create(&thread, NULL, &func, &argp))) {
			/* NOTE: This is ok on linux, The implementation of
			 * errno in glibc is thread-safety. but don't all. */
			errno = err;
			err_sys("pthread_create");
		}
