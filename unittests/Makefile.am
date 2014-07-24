SOURCES = \
	BinTreeTest.cpp \
	BinTreeTest.h \
	DirIteratorTest.cpp \
	DirIteratorTest.h \
	ELFBinaryReaderTest.cpp \
	ELFBinaryReaderTest.h \
	ELFReaderTest.cpp \
	ELFReaderTest.h \
	FileHandleTest.cpp \
	FileHandleTest.h \
	FragmentRefTest.cpp \
	FragmentRefTest.h \
	FragmentTest.cpp \
	FragmentTest.h \
	GCFactoryListTraitsTest.cpp \
	GCFactoryListTraitsTest.h \
	HashTableTest.cpp \
	HashTableTest.h \
	InputTreeTest.cpp \
	InputTreeTest.h \
	LDSymbolTest.cpp \
	LDSymbolTest.h \
	LEB128Test.cpp \
	LEB128Test.h \
	LinearAllocatorTest.cpp \
	LinearAllocatorTest.h \
	LinkerTest.cpp \
	LinkerTest.h \
	PathTest.cpp \
	PathTest.h \
	RTLinearAllocatorTest.h \
	RTLinearAllocatorTest.cpp \
	SectionDataTest.cpp \
	SectionDataTest.h \
	StaticResolverTest.cpp \
	StaticResolverTest.h \
	SymbolCategoryTest.cpp \
	SymbolCategoryTest.h \
	SystemUtilsTest.cpp \
	SystemUtilsTest.h \
	UniqueGCFactoryBaseTest.cpp \
	UniqueGCFactoryBaseTest.h

ANDROID_CPPFLAGS=-fno-rtti -fno-exceptions -Waddress -Wchar-subscripts -Wcomment -Wformat -Wparentheses -Wreorder -Wreturn-type -Wsequence-point -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wvolatile-register-var -Wsign-compare -Werror

MCLD_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include $(LLVM_CPPFLAGS) $(ANDROID_CPPFLAGS) -I$(top_srcdir)/include/mcld/Script -I$(srcdir)/include -I$(top_srcdir)/utils/gtest/include -DTOPDIR=\"$(abs_top_srcdir)\" -I$(top_srcdir)/unittests -DGTEST_HAS_RTTI=0

if ENABLE_OPTIMIZED
MCLD_CPPFLAGS+=-O2
else
MCLD_CPPFLAGS+=-g
endif

noinst_PROGRAMS = MCLDUnittests

AM_CPPFLAGS = $(MCLD_CPPFLAGS)

MCLDUnittests_LDFLAGS = \
	$(top_builddir)/lib/libmcld.a \
	-L$(top_builddir)/utils/gtest -lgtest \
	-L$(top_builddir)/utils/gtestmain -lgtestmain \
	$(LLVM_LDFLAGS)

dist_MCLDUnittests_SOURCES = $(SOURCES)

MCLD = $(top_builddir)/lib/libmcld.a
GTEST = $(top_builddir)/utils/gtest/libgtest.la
GTESTMAIN = $(top_builddir)/utils/gtestmain/libgtestmain.la

unittests: $(noinst_PROGRAMS)
	$(abs_builddir)/$(noinst_PROGRAMS)

$(noinst_PROGRAMS): $(MCLD) $(GTEST) $(GTESTMAIN)

$(MCLD):
	cd $(top_builddir)/lib && $(MAKE) $(AM_MAKEFLAGS)

$(GTEST):
	cd $(top_builddir)/utils/gtest && $(MAKE) $(AM_MAKEFLAGS)

$(GTESTMAIN):
	cd $(top_builddir)/utils/gtestmain && $(MAKE) $(AM_MAKEFLAGS)
