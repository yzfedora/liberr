#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([liberr], [1.0], [yzfedora@gmail.com])
AM_INIT_AUTOMAKE
LT_INIT
AC_CONFIG_SRCDIR([err_handler.c])
AC_CONFIG_HEADERS([config.h])


# Checks for programs.
# AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lerr':
# AC_CHECK_LIB([err], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [pthread_exit])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([dup2 strchr strerror strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
