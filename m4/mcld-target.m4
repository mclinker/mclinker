dnl
dnl @synopsis CHECK_ALONE_TARGET
dnl
dnl @summary enumlate stand alone targets. set up variables:
dnl   CHECK_TARGET
dnl
dnl Luba Tang <lubatang@gmail.com>

AH_TEMPLATE([TARGET_BUILD], [Define for standalone Android linker])

AC_DEFUN([CHECK_ALONE_TARGET],
[dnl
	PROVIDE_ALONE_TARGET="";
	DEFAULT_ALONE_TARGET="";

	AC_ARG_ENABLE([alone-target],
		[AS_HELP_STRING([--enable-alone-target],
				[Build specific target. Valid target is one of:
	     host, x86, x86_64, sparc, powerpc, alpha, arm, mips, spu, hexagon,
	     xcore, msp430, systemz, blackfin, ptx, cbe, and cpp (default=host)])],
		[],
		[enableval=host])

	case "$enableval" in
	  host)
		case "$host" in
		  x86-*-* | i*86-*-*)
			PROVIDE_ALONE_TARGET="X86"
			DEFAULT_ALONE_TARGET="X86"
			;;
		  x86_64-*-* | amd64-*-*)
			PROVIDE_ALONE_TARGET="X86"
			DEFAULT_ALONE_TARGET="X86_64"
			;;
		  arm*-*-*)
			PROVIDE_ALONE_TARGET="ARM"
			DEFAULT_ALONE_TARGET="ARM"
			;;
		  mips*-*-*)
			PROVIDE_ALONE_TARGET="MIPS"
			DEFAULT_ALONE_TARGET="MIPS"
			;;
	  	  *) AC_MSG_ERROR([Unrecognized host `$host']) ;;
		esac
		;;

	  x86)
		PROVIDE_ALONE_TARGET="X86"
		DEFAULT_ALONE_TARGET="X86"
		AC_DEFINE(TARGET_BUILD)
		;;
	  x86_64)
		PROVIDE_ALONE_TARGET="X86"
		DEFAULT_ALONE_TARGET="X86_64"
		AC_DEFINE(TARGET_BUILD)
		;;
	  arm)
		PROVIDE_ALONE_TARGET="ARM"
		DEFAULT_ALONE_TARGET="ARM"
		AC_DEFINE(TARGET_BUILD)
		;;
	  mips)
		PROVIDE_ALONE_TARGET="MIPS"
		DEFAULT_ALONE_TARGET="MIPS"
		AC_DEFINE(TARGET_BUILD)
		;;
	  hexagon)
		PROVIDE_ALONE_TARGET="HEXAGON"
		DEFAULT_ALONE_TARGET="HEXAGON"
		AC_DEFINE(TARGET_BUILD)
		;;
	  *) AC_MSG_ERROR([Unrecognized target $enableval]) ;;
	esac

	AC_SUBST(PROVIDE_ALONE_TARGET,$PROVIDE_ALONE_TARGET)
	AC_SUBST(DEFAULT_ALONE_TARGET,$DEFAULT_ALONE_TARGET)
])

dnl
dnl @synopsis ENUM_MCLD_TARGETS
dnl
dnl @summary enumlate MCLD Targets, set up variables:
dnl   MCLD_ENUM_TARGETS
dnl   MCLD_ENUM_LINKERS
dnl
dnl Luba Tang <lubatang@mediatek.com>

AC_DEFUN([ENUM_MCLD_TARGETS],
[dnl
	dnl from ${LLVM}/autoconf/configure.ac
	dnl Allow specific targets to be specified for building (or not)
	TARGETS_TO_BUILD="";

	AC_ARG_ENABLE([targets],
		[AS_HELP_STRING([--enable-targets],
			        [Build specific host targets: all or target1,target2,... Valid targets are:
	     host, x86, x86_64, sparc, powerpc, alpha, arm, mips, spu, hexagon, 
	     xcore, msp430, systemz, blackfin, ptx, cbe, and cpp (default=all)])],
		[],
		[enableval=all])

	case "$enableval" in
	  all) TARGETS_TO_BUILD="X86 Sparc PowerPC Alpha ARM Mips Hexagon CellSPU XCore MSP430 SystemZ Blackfin CBackend CppBackend MBlaze PTX" ;;
	  *)for a_target in `echo $enableval|sed -e 's/,/ /g' ` ; do
	      case "$a_target" in
		x86)      TARGETS_TO_BUILD="X86 $TARGETS_TO_BUILD" ;;
		x86_64)   TARGETS_TO_BUILD="X86 $TARGETS_TO_BUILD" ;;
		sparc)    TARGETS_TO_BUILD="Sparc $TARGETS_TO_BUILD" ;;
		powerpc)  TARGETS_TO_BUILD="PowerPC $TARGETS_TO_BUILD" ;;
		alpha)    TARGETS_TO_BUILD="Alpha $TARGETS_TO_BUILD" ;;
		arm)      TARGETS_TO_BUILD="ARM $TARGETS_TO_BUILD" ;;
		mips)     TARGETS_TO_BUILD="Mips $TARGETS_TO_BUILD" ;;
		hexagon)  TARGETS_TO_BUILD="Hexagon $TARGETS_TO_BUILD" ;;
		spu)      TARGETS_TO_BUILD="CellSPU $TARGETS_TO_BUILD" ;;
		xcore)    TARGETS_TO_BUILD="XCore $TARGETS_TO_BUILD" ;;
		msp430)   TARGETS_TO_BUILD="MSP430 $TARGETS_TO_BUILD" ;;
		systemz)  TARGETS_TO_BUILD="SystemZ $TARGETS_TO_BUILD" ;;
		blackfin) TARGETS_TO_BUILD="Blackfin $TARGETS_TO_BUILD" ;;
		cbe)      TARGETS_TO_BUILD="CBackend $TARGETS_TO_BUILD" ;;
		cpp)      TARGETS_TO_BUILD="CppBackend $TARGETS_TO_BUILD" ;;
		mblaze)   TARGETS_TO_BUILD="MBlaze $TARGETS_TO_BUILD" ;;
		ptx)      TARGETS_TO_BUILD="PTX $TARGETS_TO_BUILD" ;;
		*) AC_MSG_ERROR([Unrecognized target $a_target]) ;;
	      esac
	  done
	  ;;
	esac
	AC_SUBST(TARGETS_TO_BUILD,$TARGETS_TO_BUILD)

	dnl Build the MCLD_TARGET and MCLD_* macros for Targets.def and the individual
	dnl target feature def files.
	MCLD_ENUM_TARGETS=""
	MCLD_ENUM_LINKERS=""
	for target_to_build in $TARGETS_TO_BUILD; do
	  if test -d ${srcdir}/lib/Target/${target_to_build} ; then
	    MCLD_ENUM_TARGETS="MCLD_TARGET($target_to_build) $MCLD_ENUM_TARGETS"
	  fi
	  if test -f ${srcdir}/lib/Target/${target_to_build}/*LDBackend.cpp ; then
	    MCLD_ENUM_LINKERS="MCLD_LINKER($target_to_build) $MCLD_ENUM_LINKERS";
	  fi
	done
	AC_SUBST(MCLD_ENUM_TARGETS)
	AC_SUBST(MCLD_ENUM_LINKERS)
])
