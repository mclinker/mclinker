##############################################################################
#   Copyright (C) 2011-
#   Embedded and Web Computing Lab, National Taiwan University
#   MediaTek, Inc.
#

dnl
dnl @synopsis CHECK_LLVM([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl
dnl @summary check LLVM, set up variables:
dnl   LLVM_CFLAGS="`${LLVM_CONFIG_BIN} --cflags`"
dnl   LLVM_CPPFLAGS="`${LLVM_CONFIG_BIN} --cxxflags`"
dnl   LLVM_LDFLAGS="`${LLVM_CONFIG_BIN} --ldflags --libs`"
dnl
dnl Luba Tang <lubatang@gmail.com>


AC_DEFUN([CHECK_LLVM],
[dnl

	AC_ARG_WITH(
		[llvm-config],
		[AS_HELP_STRING([--with-llvm-config[[=PATH]]], 
			[path to llvm-config (by default, searching in $PATH)])],
		[llvm_config_path="${withval}"],
		[llvm_config_path="/usr/"])

	#Set up ${LLVM_CONFIG_BIN}
	AC_MSG_CHECKING(for llvm-config)

	if test -x "${llvm_config_path}" -a ! -d "${llvm_config_path}"; then
		LLVM_CONFIG_BIN=${llvm_config_path}
		AC_MSG_RESULT([yes])
	else
		AC_MSG_RESULT([no])
		llvm_config_path=${PATH}
		if test -d "${LLVM_BINDIR}"; then
			llvm_config_path="${llvm_config_path}:${LLVM_BINDIR}"
		fi
		AC_PATH_PROG(LLVM_CONFIG_BIN, llvm-config, [no], ${llvm_config_path})
		if test "${LLVM_CONFIG_BIN}" = "no"; then
			ifelse([$3], , , [$3])
			AC_MSG_NOTICE([*** The 'llvm-config' is not found!])
			AC_MSG_ERROR([*** Please use  --with-llvm-config option with the full path of 'llvm-config'.])
		fi
	fi

	dnl Use llvm-config to do:
	dnl   1. is the minimum version correct?
	dnl   2. the includedir
	dnl   3. the flags - cflags, cxxflags, cppflags, ldflags
	dnl   4. the libs
	AC_MSG_CHECKING(for llvm - version >= $1)
	cur_version="`${LLVM_CONFIG_BIN} --version`";
	tool_major="`${LLVM_CONFIG_BIN} --version | sed 's/svn//' | sed 's/\([[0-9]]*\).\([[0-9]]*\)/\1/'`"
	tool_minor="`${LLVM_CONFIG_BIN} --version | sed 's/svn//'| sed 's/\([[0-9]]*\).\([[0-9]]*\)/\2/'`"

	require_major="`echo $1 | sed 's/svn//' | sed 's/\([[0-9]]*\).\([[0-9]]*\)/\1/'`"
	require_minor="`echo $1 | sed 's/svn//' | sed 's/\([[0-9]]*\).\([[0-9]]*\)/\2/'`"

	if test "${tool_major}" -lt "${require_major}"; then
		AC_MSG_RESULT([no])
		AC_MSG_ERROR([*** The version of LLVM is too low! (${cur_version}<$1)]) 
	elif test "${tool_major}" -eq "${require_major}"; then
		if test "${tool_minor}" -lt "${require_minor}"; then
			AC_MSG_RESULT([no])
			AC_MSG_ERROR([*** The version of LLVM is too low! (${cur_version}<$1)]) 
		fi
	fi
	AC_MSG_RESULT([yes]) 

	LLVM_CFLAGS="`${LLVM_CONFIG_BIN} --cflags`"
	LLVM_CPPFLAGS="`${LLVM_CONFIG_BIN} --cxxflags`"
	LLVM_LDFLAGS="`${LLVM_CONFIG_BIN} --ldflags --libs`"
	LLVM_LDFLAGS="`echo ${LLVM_LDFLAGS} | sed 's/\n//g'`"
	LLVM_VERSION=${tool_major}
	
	AC_SUBST(LLVM_CFLAGS)
	AC_SUBST(LLVM_CPPFLAGS)
	AC_SUBST(LLVM_LDFLAGS)
	AC_SUBST(LLVM_VERSION)
	ifelse([$2], , , [$2])
])
