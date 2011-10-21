dnl
dnl @synopsis ENUM_LLVM_TARGETS
dnl
dnl @summary enumlate LLVM Targets, set up variables:
dnl   LLVM_ENUM_TARGETS
dnl   LLVM_ENUM_LINKERS
dnl
dnl Luba Tang <lubatang@mediatek.com>

AC_DEFUN([ENUM_LLVM_TARGETS],
[dnl
	dnl from ${LLVM}/autoconf/configure.ac
	dnl Allow specific targets to be specified for building (or not)
	TARGETS_TO_BUILD="";

	AC_ARG_ENABLE([targets],
		[AS_HELP_STRING([--enable-targets],
			        [Build specific host targets: all or target1,target2,... Valid targets are:
	     host, x86, x86_64, sparc, powerpc, alpha, arm, mips, spu,
	     xcore, msp430, systemz, blackfin, ptx, cbe, and cpp (default=all)])],
		[],
		[enableval=all])

	case "$enableval" in
	  all) TARGETS_TO_BUILD="X86 Sparc PowerPC Alpha ARM Mips CellSPU XCore MSP430 SystemZ Blackfin CBackend CppBackend MBlaze PTX" ;;
	  *)for a_target in `echo $enableval|sed -e 's/,/ /g' ` ; do
	      case "$a_target" in
		x86)      TARGETS_TO_BUILD="X86 $TARGETS_TO_BUILD" ;;
		x86_64)   TARGETS_TO_BUILD="X86 $TARGETS_TO_BUILD" ;;
		sparc)    TARGETS_TO_BUILD="Sparc $TARGETS_TO_BUILD" ;;
		powerpc)  TARGETS_TO_BUILD="PowerPC $TARGETS_TO_BUILD" ;;
		alpha)    TARGETS_TO_BUILD="Alpha $TARGETS_TO_BUILD" ;;
		arm)      TARGETS_TO_BUILD="ARM $TARGETS_TO_BUILD" ;;
		mips)     TARGETS_TO_BUILD="Mips $TARGETS_TO_BUILD" ;;
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

	dnl Build the LLVM_TARGET and LLVM_... macros for Targets.def and the individual
	dnl target feature def files.
	LLVM_ENUM_TARGETS=""
	LLVM_ENUM_LINKERS=""
	for target_to_build in $TARGETS_TO_BUILD; do
	  if test -d ${srcdir}/lib/Target/${target_to_build} ; then
	    LLVM_ENUM_TARGETS="LLVM_TARGET($target_to_build) $LLVM_ENUM_TARGETS"
	  fi
	  if test -f ${srcdir}/lib/Target/${target_to_build}/*LDBackend.cpp ; then
	    LLVM_ENUM_LINKERS="LLVM_LINKER($target_to_build) $LLVM_ENUM_LINKERS";
	  fi
	done
	AC_SUBST(LLVM_ENUM_TARGETS)
	AC_SUBST(LLVM_ENUM_LINKERS)
])
