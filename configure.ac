
AC_PREREQ([2.69])
AC_INIT([cryptsetup-gui], [0.6.1], [https://github.com/chron0/cryptsetup-gui])
AC_CONFIG_SRCDIR([src/cryptsetup-gui.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])


# Checks for programs.
AC_PROG_CC

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_UID_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strstr])

PKG_CHECK_MODULES([gtk], [gtk+-2.0])

AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_OUTPUT
