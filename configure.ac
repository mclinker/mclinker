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

dnl AC_INIT(PKG_NAME, CONFIG_VERSION, BUG_REPORTER)
AC_INIT([MCLinker],[1.1],[mclinker@googlegroups.com])
AC_PREREQ(2.59)

AC_CONFIG_SRCDIR([./COPYING])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.9 dist-bzip2 foreign])

####################
#  Versioning, see VERSION
MAJOR=0
MINOR=2
MICRO=10
AGE=1
BUILD=18peaks

DEV_VERSION=$MAJOR.$MINOR.$MICRO.$AGE-$BUILD
PKG_VERSION=$MAJOR.$MINOR.$MICRO-$BUILD
MCLD_VERSION=$DEV_VERSION
AC_SUBST(MCLD_VERSION)

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
# Configure LLVM
CHECK_LLVM([3.0])
ENUM_LLVM_TARGETS

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
AC_CONFIG_FILES([include/mcld/Config/Config.h])
AC_CONFIG_FILES([test/Makefile])

AC_OUTPUT
