INCDIR=$(srcdir)/include/mcld
LIBDIR=$(srcdir)/lib

MCLD_SOURCES = $(srcdir)/main.cpp \
	$(INCDIR)/DynamicSectionOptions.h \
	$(LIBDIR)/DynamicSectionOptions.cpp \
	$(INCDIR)/OptimizationOptions.h \
	$(LIBDIR)/OptimizationOptions.cpp \
	$(INCDIR)/OutputFormatOptions.h \
	$(LIBDIR)/OutputFormatOptions.cpp \
	$(INCDIR)/PositionalOptions.h \
	$(LIBDIR)/PositionalOptions.cpp \
	$(INCDIR)/PreferenceOptions.h \
	$(LIBDIR)/PreferenceOptions.cpp \
	$(INCDIR)/ScriptOptions.h \
	$(LIBDIR)/ScriptOptions.cpp \
	$(INCDIR)/SearchPathOptions.h \
	$(LIBDIR)/SearchPathOptions.cpp \
	$(INCDIR)/SymbolOptions.h \
	$(LIBDIR)/SymbolOptions.cpp \
	$(INCDIR)/TargetControlOptions.h \
	$(LIBDIR)/TargetControlOptions.cpp \
	$(INCDIR)/TripleOptions.h \
	$(LIBDIR)/TripleOptions.cpp

ANDROID_CPPFLAGS=-fno-rtti -fno-exceptions -Waddress -Wchar-subscripts -Wcomment -Wformat -Wparentheses -Wreorder -Wreturn-type -Wsequence-point -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wvolatile-register-var

MCLD_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include $(LLVM_CPPFLAGS) $(ANDROID_CPPFLAGS) -I$(top_srcdir)/include/mcld/Script -I$(srcdir)/include

if ENABLE_OPTIMIZED
MCLD_CPPFLAGS+=-O2
else
MCLD_CPPFLAGS+=-g
endif

if ENABLE_WERROR
MCLD_CPPFLAGS+=-Werror
endif

bin_PROGRAMS = ld.mcld

AM_CPPFLAGS = $(MCLD_CPPFLAGS)

ld_mcld_LDFLAGS = $(LLVM_LDFLAGS)
if ENABLE_OPTIMIZED
MCLD_DIR = optimized
else
MCLD_DIR = debug
endif
ld_mcld_LDADD = $(top_builddir)/$(MCLD_DIR)/libmcld.a

dist_ld_mcld_SOURCES = $(MCLD_SOURCES)
