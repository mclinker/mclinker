ANDROID_CPPFLAGS=-fno-rtti -fno-exceptions -Waddress -Wchar-subscripts -Wcomment -Wformat -Wparentheses -Wreorder -Wreturn-type -Wsequence-point -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wvolatile-register-var

MCLD_CPPFLAGS = -g ${LLVM_CPPFLAGS} ${ANDROID_CPPFLAGS}

if ENABLE_WERROR
MCLD_CPPFLAGS+=-Werror
endif

MCLD_INCLUDES = -I${top_srcdir}/include \
	-I${top_builddir}/include \
	-I${builddir}/include \
	-I${srcdir}/include

bin_PROGRAMS = ld.bcc

AM_CPPFLAGS = ${MCLD_INCLUDES} ${MCLD_CPPFLAGS}

ld_bcc_LDFLAGS = ${LLVM_LDFLAGS}

if ENABLE_UNITTEST
ld_bcc_LDADD = ${top_builddir}/debug/libmcld.a
else
ld_bcc_LDADD = ${top_builddir}/optimized/libmcld.a
endif

dist_ld_bcc_SOURCES = ${srcdir}/main.cpp \
    ${srcdir}/include/bcc/Linker.h \
    ${srcdir}/lib/Core/Linker.cpp \
    ${srcdir}/include/bcc/Support/LinkerConfig.h \
    ${srcdir}/lib/Support/LinkerConfig.cpp \
    ${srcdir}/include/bcc/Support/TargetLinkerConfigs.h \
    ${srcdir}/lib/Support/TargetLinkerConfigs.cpp \
    ${srcdir}/include/bcc/Support/Initialization.h \
    ${srcdir}/lib/Support/Initialization.cpp \
    ${srcdir}/include/bcc/Support/Log.h
