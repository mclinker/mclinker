#   Copyright (C) 2011-
#   Embedded and Web Computing Lab, National Taiwan University
#   MediaTek, Inc.
#

##############################################################################
### WARNING: this file contains embedded tabs.  Do not run untabify on this file.

####################
#  m4 includes
m4_include(m4/acx_pthread.m4)

####################
#  Versioning, see VERSION
MAJOR=0
MINOR=0
CHANGE=0
AGE=0

DEV_VERSION="$MAJOR.$MINOR.$CHANGE.$AGE"
PKG_VERSION="$MAJOR.$MINOR.$CHANGE"


####################
#  Autoconf/Automake Initialization

AC_INIT([MCLinker],[0.0.0],[lubatang@mediatek.com])
AC_PREREQ(2.59)

AC_CONFIG_SRCDIR([./COPYING])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.9 dist-bzip2 foreign])

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
CHECK_LLVM([2.9])
ENUM_LLVM_TARGETS

####################
# OUTPUT
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([debug/Makefile])
AC_CONFIG_FILES([optimized/Makefile])
AC_CONFIG_FILES([utils/Makefile])
AC_CONFIG_FILES([utils/gtest/Makefile])
AC_CONFIG_FILES([utils/gtest/obj/Makefile])
AC_CONFIG_FILES([include/mcld/Config/Targets.def])
AC_CONFIG_FILES([include/mcld/Config/Linkers.def])

AC_CONFIG_FILES([tools/MCObjectReader/Makefile])

AC_OUTPUT
