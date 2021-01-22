#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Fix ToUnicode CMap in PDF], [0.0.0], ,
    [pdf-fix-tuc], [https://github.com/trueroad/pdf-fix-tuc])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([config.h])

PACKAGE_COPYRIGHT="Copyright (C) 2021 Masamichi Hosoda. All rights reserved."
PACKAGE_LICENSE="License: BSD-2-Clause"

AC_SUBST([PACKAGE_COPYRIGHT])
AC_SUBST([PACKAGE_LICENSE])
AC_DEFINE_UNQUOTED([PACKAGE_COPYRIGHT], ["$PACKAGE_COPYRIGHT"],
    [Define to the copyright of this package.])
AC_DEFINE_UNQUOTED([PACKAGE_LICENSE], ["$PACKAGE_LICENSE"],
    [Define to the license of this package.])

# Checks for programs.
AC_PROG_CXX
AC_PATH_PROG([WGET], [wget])
AC_SUBST([WGET])

AC_ARG_ENABLE([build-table],
    [AS_HELP_STRING([--enable-build-table],
        [Enable building table (default is disable)])],
    [enable_build_table=yes],
    [enable_build_table=no])
AM_CONDITIONAL([ENABLE_BUILD_TABLE], [test "x$enable_build_table" = xyes])

# Checks for libraries.
PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([LIBQPDF], [libqpdf])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AX_CXX_COMPILE_STDCXX(11, [noext], [mandatory])

# Checks for library functions.

AC_CONFIG_FILES([Makefile
    src/Makefile])
AC_OUTPUT
