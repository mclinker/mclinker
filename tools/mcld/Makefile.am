BUILT_SOURCES = Options.inc

MCLD_SOURCES = Main.cpp

ANDROID_CPPFLAGS=-fno-rtti -fno-exceptions -Waddress -Wchar-subscripts -Wcomment -Wformat -Wparentheses -Wreorder -Wreturn-type -Wsequence-point -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wvolatile-register-var -Wsign-compare -Werror

MCLD_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include $(LLVM_CPPFLAGS) $(ANDROID_CPPFLAGS) -I$(srcdir)/include

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
	$(LLVM_LDFLAGS) \
	-L$(top_builddir)/utils/zlib -lcrc

MCLD = $(top_builddir)/lib/libmcld.a
CRCLIB = $(top_builddir)/utils/zlib/libcrc.la

$(bin_PROGRAMS): $(MCLD) $(CRCLIB)

$(CRCLIB):
	cd $(top_builddir)/utils/zlib && $(MAKE) $(AM_MAKEFLAGS)

Options.inc : Options.td $(LLVM_TBLGEN)
	@$(LLVM_TBLGEN) -gen-opt-parser-defs -I $(LLVM_SRC_ROOT)/include -o $@ $<
