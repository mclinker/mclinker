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
	LLVM_LDFLAGS="`${LLVM_CONFIG_BIN} --libs`"
	LLVM_LDFLAGS="${LLVM_LDFLAGS} `${LLVM_CONFIG_BIN} --ldflags`"
	LLVM_LDFLAGS="`echo ${LLVM_LDFLAGS} | sed 's/\n//g'`"
	LLVM_LDFLAGS="`echo ${LLVM_LDFLAGS} | sed 's/-lgtest_main -lgtest//g'`"
	
	AC_SUBST(LLVM_CFLAGS)
	AC_SUBST(LLVM_CPPFLAGS)
	AC_SUBST(LLVM_LDFLAGS)
	ifelse([$2], , , [$2])

	AC_CACHE_CHECK([type of operating system we're going to host on],
			[llvm_cv_os_type],
			[case $host in
			*-*-aix*)
				llvm_cv_os_type="AIX"
				llvm_cv_platform_type="Unix" ;;
			*-*-irix*)
				llvm_cv_os_type="IRIX"
				llvm_cv_platform_type="Unix" ;;
			*-*-cygwin*)
				llvm_cv_os_type="Cygwin"
				llvm_cv_platform_type="Unix" ;;
			*-*-darwin*)
				llvm_cv_os_type="Darwin"
				llvm_cv_platform_type="Unix" ;;
			*-*-minix*)
				llvm_cv_os_type="Minix"
				llvm_cv_platform_type="Unix" ;;
			*-*-freebsd*)
				llvm_cv_os_type="FreeBSD"
				llvm_cv_platform_type="Unix" ;;
			*-*-openbsd*)
				llvm_cv_os_type="OpenBSD"
				llvm_cv_platform_type="Unix" ;;
			*-*-netbsd*)
				llvm_cv_os_type="NetBSD"
				llvm_cv_platform_type="Unix" ;;
			*-*-dragonfly*)
				llvm_cv_os_type="DragonFly"
				llvm_cv_platform_type="Unix" ;;
			*-*-hpux*)
				llvm_cv_os_type="HP-UX"
				llvm_cv_platform_type="Unix" ;;
			*-*-interix*)
				llvm_cv_os_type="Interix"
				llvm_cv_platform_type="Unix" ;;
			*-*-linux*)
				llvm_cv_os_type="Linux"
				llvm_cv_platform_type="Unix" ;;
			*-*-solaris*)
				llvm_cv_os_type="SunOS"
				llvm_cv_platform_type="Unix" ;;
			*-*-auroraux*)
				llvm_cv_os_type="AuroraUX"
				llvm_cv_platform_type="Unix" ;;
			*-*-win32*)
				llvm_cv_os_type="Win32"
				llvm_cv_platform_type="Win32" ;;
			*-*-mingw*)
				llvm_cv_os_type="MingW"
				llvm_cv_platform_type="Win32" ;;
			*-*-haiku*)
				llvm_cv_os_type="Haiku"
				llvm_cv_platform_type="Unix" ;;
			*-unknown-eabi*)
				llvm_cv_os_type="Freestanding"
				llvm_cv_platform_type="Unix" ;;
			*-unknown-elf*)
				llvm_cv_os_type="Freestanding"
				llvm_cv_platform_type="Unix" ;;
			*)
				llvm_cv_os_type="Unknown"
				llvm_cv_platform_type="Unknown" ;;
			esac])

	dnl Set the "OS" Makefile variable based on the platform type so the
	dnl makefile can configure itself to specific build hosts
	if test "$llvm_cv_os_type" = "Unknown" ; then
		AC_MSG_ERROR([Operating system is unknown, configure can't continue])
	fi

	AC_SUBST(HOST_OS,$llvm_cv_os_type)
	AC_SUBST(TARGET_OS,$llvm_cv_target_os_type)

	dnl Set the "MCLD_ON_*" variables based on llvm_cv_llvm_cv_platform_type
	dnl This is used by lib/Support to determine the basic kind of implementation
	dnl to use.
	case $llvm_cv_platform_type in
	Unix)
		AC_DEFINE([MCLD_ON_UNIX],[1],[Define if this is Unixish platform])
		AC_SUBST(MCLD_ON_PLATFORM,[MCLD_ON_UNIX])
	;;
	Win32)
		AC_DEFINE([MCLD_ON_WIN32],[1],[Define if this is Win32ish platform])
		AC_SUBST(MCLD_ON_PLATFORM,[MCLD_ON_WIN32])
	;;
	esac

	AC_CACHE_CHECK([type of operating system we're going to target],
	               [llvm_cv_target_os_type],
	[case $target in
	  *-*-aix*)
	    llvm_cv_target_os_type="AIX" ;;
	  *-*-irix*)
	    llvm_cv_target_os_type="IRIX" ;;
	  *-*-cygwin*)
	    llvm_cv_target_os_type="Cygwin" ;;
	  *-*-darwin*)
	    llvm_cv_target_os_type="Darwin" ;;
	  *-*-minix*)
	    llvm_cv_target_os_type="Minix" ;;
	  *-*-freebsd* | *-*-kfreebsd-gnu)
	    llvm_cv_target_os_type="FreeBSD" ;;
	  *-*-openbsd*)
	    llvm_cv_target_os_type="OpenBSD" ;;
	  *-*-netbsd*)
	    llvm_cv_target_os_type="NetBSD" ;;
	  *-*-dragonfly*)
	    llvm_cv_target_os_type="DragonFly" ;;
	  *-*-hpux*)
	    llvm_cv_target_os_type="HP-UX" ;;
	  *-*-interix*)
	    llvm_cv_target_os_type="Interix" ;;
	  *-*-linux*)
	    llvm_cv_target_os_type="Linux" ;;
	  *-*-gnu*)
	    llvm_cv_target_os_type="GNU" ;;
	  *-*-solaris*)
	    llvm_cv_target_os_type="SunOS" ;;
	  *-*-auroraux*)
	    llvm_cv_target_os_type="AuroraUX" ;;
	  *-*-win32*)
	    llvm_cv_target_os_type="Win32" ;;
	  *-*-mingw*)
	    llvm_cv_target_os_type="MingW" ;;
	  *-*-haiku*)
	    llvm_cv_target_os_type="Haiku" ;;
	  *-*-rtems*)
	    llvm_cv_target_os_type="RTEMS" ;;
	  *-*-nacl*)
	    llvm_cv_target_os_type="NativeClient" ;;
	  *-unknown-eabi*)
	    llvm_cv_target_os_type="Freestanding" ;;
	  *)
	    llvm_cv_target_os_type="Unknown" ;;
	esac])

	dnl Determine what our target architecture is and configure accordingly.
	dnl This will allow Makefiles to make a distinction between the hardware and
	dnl the OS.
	AC_CACHE_CHECK([target architecture],[llvm_cv_target_arch],
	[case $target in
	  i?86-*)                 llvm_cv_target_arch="x86" ;;
	  amd64-* | x86_64-*)     llvm_cv_target_arch="x86_64" ;;
	  sparc*-*)               llvm_cv_target_arch="Sparc" ;;
	  powerpc*-*)             llvm_cv_target_arch="PowerPC" ;;
	  arm*-*)                 llvm_cv_target_arch="ARM" ;;
	  aarch64*-*)             llvm_cv_target_arch="AArch64" ;;
	  mips-* | mips64-*)      llvm_cv_target_arch="Mips" ;;
	  mipsel-* | mips64el-*)  llvm_cv_target_arch="Mips" ;;
	  xcore-*)                llvm_cv_target_arch="XCore" ;;
	  msp430-*)               llvm_cv_target_arch="MSP430" ;;
	  hexagon-*)              llvm_cv_target_arch="Hexagon" ;;
	  mblaze-*)               llvm_cv_target_arch="MBlaze" ;;
	  nvptx-*)                llvm_cv_target_arch="NVPTX" ;;
	  *)                      llvm_cv_target_arch="Unknown" ;;
	esac])

	if test "$llvm_cv_target_arch" = "Unknown" ; then
	  AC_MSG_WARN([Configuring MCLINKER for an unknown target archicture])
	fi

	dnl Set the "MCLD_DEFAULT_TARGET_TRIPLE" variable based on $target.
	dnl This is used to determine the default target triple and emulation
	dnl to use.
	AC_DEFINE_UNQUOTED([MCLD_DEFAULT_TARGET_TRIPLE], ["$target"],
                           [Target triple MCLinker will generate code for by default])
])
