INCDIR=${srcdir}/include/lite
LIBDIR=${srcdir}/lib

MCLD_SOURCES = ${srcdir}/main.cpp \
	${INCDIR}/DynamicSectionOptions.h \
	${LIBDIR}/DynamicSectionOptions.cpp \
	${INCDIR}/OptimizationOptions.h \
	${LIBDIR}/OptimizationOptions.cpp \
	${INCDIR}/OutputFormatOptions.h \
	${LIBDIR}/OutputFormatOptions.cpp \
	${INCDIR}/PositionalOptions.h \
	${LIBDIR}/PositionalOptions.cpp \
	${INCDIR}/PreferenceOptions.h \
	${LIBDIR}/PreferenceOptions.cpp \
	${INCDIR}/ScriptOptions.h \
	${LIBDIR}/ScriptOptions.cpp \
	${INCDIR}/SearchPathOptions.h \
	${LIBDIR}/SearchPathOptions.cpp \
	${INCDIR}/SymbolOptions.h \
	${LIBDIR}/SymbolOptions.cpp \
	${INCDIR}/TargetControlOptions.h \
	${LIBDIR}/TargetControlOptions.cpp \
	${INCDIR}/TripleOptions.h \
	${LIBDIR}/TripleOptions.cpp

ANDROID_CPPFLAGS=-fno-rtti -fno-exceptions -Waddress -Wchar-subscripts -Wcomment -Wformat -Wparentheses -Wreorder -Wreturn-type -Wsequence-point -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wvolatile-register-var

MCLD_CPPFLAGS = -g -I${top_srcdir}/include -I${top_builddir}/include ${LLVM_CPPFLAGS} ${ANDROID_CPPFLAGS} -I${top_srcdir}/include/mcld/Script -I${srcdir}/include

if ENABLE_WERROR
MCLD_CPPFLAGS+=-Werror
endif

bin_PROGRAMS = ld.lite

if ENABLE_UNITTEST
MCLD_CPPFLAGS += -DTOPDIR=\"${abs_top_srcdir}\" -DENABLE_UNITTEST -DMCLD_DEBUG -I${top_srcdir}/utils/gtest/include -I${top_srcdir}/unittests -DGTEST_HAS_RTTI=0
endif

AM_CPPFLAGS = ${MCLD_CPPFLAGS}

if ENABLE_UNITTEST
ld_lite_LDFLAGS = -L${top_builddir}/utils/gtest/lib -lgtest ${LLVM_LDFLAGS}
ld_lite_LDADD = ${top_builddir}/debug/libmcld.a
else
ld_lite_LDFLAGS = ${LLVM_LDFLAGS}
ld_lite_LDADD = ${top_builddir}/optimized/libmcld.a
endif

dist_ld_lite_SOURCES = ${MCLD_SOURCES}
