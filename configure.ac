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

AC_PATH_PROG([DIFF], [diff])
AS_IF([test "x$DIFF" = x],
      [AC_MSG_WARN([diff isn't found. `make check' will fail tests.])])
AC_SUBST([DIFF])

AC_PATH_PROG([PDFTOTEXT], [pdftotext])
AS_IF([test "x$PDFTOTEXT" = x],
      [AC_MSG_WARN([pdftotext isn't found. `make check' will fail tests.])])
AC_SUBST([PDFTOTEXT])

AC_MSG_CHECKING([for pre-generated test PDFs])
AS_IF([test ! -e ${srcdir}/test/test.pdf],
      [AC_MSG_RESULT([no])
       AC_PATH_PROG([UPTEX], [uptex])
       AC_PATH_PROG([DVIPDFMX], [dvipdfmx])
       AS_IF([test "x$UPTEX" = x || test "x$DVIPDFMX" = x],
             [AC_MSG_WARN(
                  [uptex/dvipdfmx isn't found. Test PDFs cannot be generated.])
              AC_MSG_WARN(
                  [You cannot use `make check'.])],
             [AC_MSG_NOTICE(
                  [uptex/dvipdfmx is found. Test PDFs will be generated.])])],
      [AC_MSG_RESULT([yes])])
AC_SUBST([UPTEX])
AC_SUBST([DVIPDFMX])

AC_ARG_ENABLE([build-table],
    [AS_HELP_STRING([--enable-build-table],
        [Enable building table (default is disable)])],
    [enable_build_table=yes
     AC_MSG_CHECKING([for build-table source aj17-kanji.txt])
     AS_IF([test ! -e ${srcdir}/src/aj17-kanji.txt],
        [AC_MSG_RESULT([no])
         AC_PATH_PROG([WGET], [wget])
         AS_IF([test "x$WGET" = x],
            [AC_MSG_ERROR(
                [wget isn't found. aj17-kanji.txt cannot be retrieved.])],
            [AC_MSG_NOTICE(
                [wget is found. aj17-kanji.txt will be retrieved.])])],
        [AC_MSG_RESULT([yes])])
     AC_PATH_PROG([PYTHON3], [python3])
     AS_IF([test "x$PYTHON3" = x],
        [AC_MSG_ERROR([python3 isn't found. You cannot enable build-table.])],
        [AC_MSG_NOTICE([python3 is found. The table will build.])])],
    [enable_build_table=no])
AM_CONDITIONAL([ENABLE_BUILD_TABLE], [test "x$enable_build_table" = xyes])
AC_SUBST([WGET])

# Checks for libraries.
PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([LIBQPDF], [libqpdf])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AX_CXX_COMPILE_STDCXX(11, [noext], [mandatory])

# Checks for library functions.

AC_CONFIG_FILES([Makefile
    src/Makefile
    test/Makefile])
AC_OUTPUT
