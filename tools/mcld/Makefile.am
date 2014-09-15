MCLD_SOURCES = main.cpp \
	include/mcld/DynamicSectionOptions.h \
	lib/DynamicSectionOptions.cpp \
	include/mcld/OptimizationOptions.h \
	lib/OptimizationOptions.cpp \
	include/mcld/OutputFormatOptions.h \
	lib/OutputFormatOptions.cpp \
	include/mcld/PositionalOptions.h \
	lib/PositionalOptions.cpp \
	include/mcld/PreferenceOptions.h \
	lib/PreferenceOptions.cpp \
	include/mcld/ScriptOptions.h \
	lib/ScriptOptions.cpp \
	include/mcld/SearchPathOptions.h \
	lib/SearchPathOptions.cpp \
	include/mcld/SymbolOptions.h \
	lib/SymbolOptions.cpp \
	include/mcld/TargetControlOptions.h \
	lib/TargetControlOptions.cpp \
	include/mcld/TripleOptions.h \
	lib/TripleOptions.cpp

ANDROID_CPPFLAGS=-fno-rtti -fno-exceptions -Waddress -Wchar-subscripts -Wcomment -Wformat -Wparentheses -Wreorder -Wreturn-type -Wsequence-point -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wvolatile-register-var -Wsign-compare -Werror

MCLD_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include $(LLVM_CPPFLAGS) $(ANDROID_CPPFLAGS) -I$(top_srcdir)/include/mcld/Script -I$(srcdir)/include

if ENABLE_OPTIMIZED
MCLD_CPPFLAGS+=-O2
else
MCLD_CPPFLAGS+=-g
endif

bin_PROGRAMS = ld.mcld

AM_CPPFLAGS = $(MCLD_CPPFLAGS)

ld_mcld_SOURCES = $(MCLD_SOURCES)

ld_mcld_LDFLAGS = \
	$(top_builddir)/lib/libmcld.a \
	-L$(top_builddir)/utils/zlib -lcrc \
	$(LLVM_LDFLAGS)

MCLD = $(top_builddir)/lib/libmcld.a
CRCLIB = $(top_builddir)/utils/zlib/libcrc.la

$(bin_PROGRAMS): $(MCLD) $(CRCLIB)

$(CRCLIB):
	cd $(top_builddir)/utils/zlib && $(MAKE) $(AM_MAKEFLAGS)
