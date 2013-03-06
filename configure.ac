#                     The MCLinker project
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.

##############################################################################
### WARNING: this file contains embedded tabs.  Do not run untabify on this file.

####################
#  m4 includes
m4_include(m4/acx_pthread.m4)
####################
#  Autoconf/Automake Initialization

dnl AC_INIT(PKG_NAME, BUILD_VERSION, BUG_REPORTER)
AC_INIT([MCLinker],[NanHu],[mclinker@googlegroups.com])
AC_PREREQ(2.59)

AC_CONFIG_SRCDIR([./COPYING])
AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_MACRO_DIR([m4])

dnl Check the target for which we're linking and the host that will do the
dnl linking. This will tell us which MCLINKER will be used for linking
dnl input objects into output. This needs to be done early because
dnl following tests depend on it.
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([1.9 dist-bzip2 foreign])

####################
#  Versioning, see VERSION
MAJOR=2
MINOR=1
MICRO=0
AGE=0
BUILD=NanHu

DEV_VERSION=$MAJOR.$MINOR.$MICRO.$AGE-$BUILD
PKG_VERSION=$MAJOR.$MINOR.$MICRO-$BUILD
MCLD_VERSION=$DEV_VERSION
AC_SUBST(MCLD_VERSION)
AC_DEFINE_UNQUOTED([MCLD_VERSION], ["$MCLD_VERSION"])
AH_TEMPLATE([MCLD_VERSION], [MCLINKER version])

####################
#  Languages
AC_PROG_CC
AC_PROG_CXX
AC_LANG([C++])
LT_INIT

####################
#  Check for libraries

#  Configure pthreads.
AC_ARG_WITH([pthreads],
            [AS_HELP_STRING([--with-pthreads],
               [use pthreads (default is yes)])],
            [with_pthreads=$withval],
            [with_pthreads=check])

have_pthreads=no
AS_IF([test "x$with_pthreads" != "xno"],
      [ACX_PTHREAD(
        [],
        [AS_IF([test "x$with_pthreads" != "xcheck"],
               [AC_MSG_FAILURE(
                 [--with-pthreads was specified, but unable to be used])])])
       have_pthreads="$acx_pthread_ok"])
AM_CONDITIONAL([HAVE_PTHREADS],[test "x$have_pthreads" == "xyes"])
AC_SUBST(PTHREAD_CFLAGS)
AC_SUBST(PTHREAD_LIBS)

####################
# Configure Unit-test
AC_ARG_ENABLE(unittest,
            [AS_HELP_STRING([--enable-unittest],
                [turn on unit-tests (default is no)])],
            [case "${enableval}" in
              yes) unittest=true ;;
              no)  unittest=false ;;
              *) AC_MSG_ERROR(bad value ${enableval} for --enable-unittest) ;;
             esac],
             [unittest=false])
AM_CONDITIONAL([ENABLE_UNITTEST],[test "x${unittest}" == "xtrue" ])

####################
# Configure compiler warnings
CHECK_WERROR_FLAGS
AC_MSG_CHECKING([for optional compiler flags])
CXX_FLAG_CHECK(NO_VARIADIC_MACROS, [-Wno-variadic-macros])
CXX_FLAG_CHECK(NO_COVERED_SWITCH_DEFAULT, [-Wno-covered-switch-default])
CXX_FLAG_CHECK(NO_C99_EXTENSIONS, [-Wno-c99-extensions])
AC_MSG_RESULT([$NO_VARIADIC_MACROS $NO_COVERED_SWITCH_DEFAULT $NO_C99_EXTENSIONS])

####################
# Check for header files
AC_CHECK_HEADERS([fcntl.h unistd.h])
AC_CHECK_HEADERS([sys/mman.h])

####################
# Configure LLVM
CHECK_LLVM([3.3])
ENUM_MCLD_TARGETS
CHECK_ALONE_TARGET

# Config.h
AC_CONFIG_HEADER(include/mcld/Config/Config.h)

AH_TOP([
//===- Config.h.in --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_CONFIG_H
#define MCLD_CONFIG_H
])

AH_BOTTOM([
#define MCLD_REGION_CHUNK_SIZE 32
#define MCLD_NUM_OF_INPUTS 32
#define MCLD_SECTIONS_PER_INPUT 16
#define MCLD_SYMBOLS_PER_INPUT 128
#define MCLD_RELOCATIONS_PER_INPUT 1024

#endif
])

####################
# OUTPUT
AC_CONFIG_FILES([VERSION])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([debug/Makefile])
AC_CONFIG_FILES([optimized/Makefile])
AC_CONFIG_FILES([utils/Makefile])
AC_CONFIG_FILES([utils/gtest/Makefile])
AC_CONFIG_FILES([utils/gtest/obj/Makefile])
AC_CONFIG_FILES([include/mcld/Config/Targets.def])
AC_CONFIG_FILES([include/mcld/Config/Linkers.def])
AC_CONFIG_FILES([tools/mcld/include/alone/Config/Config.h])
AC_CONFIG_FILES([test/Makefile])

AC_OUTPUT
