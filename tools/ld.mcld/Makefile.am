MCLD_SOURCES = ${srcdir}/main.cpp

if ENABLE_UNITTEST
UNITTEST=${top_srcdir}/unittests

MCLD_SOURCES += \
	${UNITTEST}/PathTest.h \
	${UNITTEST}/PathTest.cpp \
	${UNITTEST}/BinTreeTest.h \
	${UNITTEST}/BinTreeTest.cpp \
	${UNITTEST}/LinearAllocatorTest.h \
	${UNITTEST}/LinearAllocatorTest.cpp \
	${UNITTEST}/InputTreeTest.h \
	${UNITTEST}/InputTreeTest.cpp \
	${UNITTEST}/RTLinearAllocatorTest.h \
	${UNITTEST}/RTLinearAllocatorTest.cpp \
	${UNITTEST}/DirIteratorTest.h \
	${UNITTEST}/DirIteratorTest.cpp \
	${UNITTEST}/UniqueGCFactoryBaseTest.h \
	${UNITTEST}/UniqueGCFactoryBaseTest.cpp \
	${UNITTEST}/MemoryAreaTest.h \
	${UNITTEST}/MemoryAreaTest.cpp \
	${UNITTEST}/HashTableTest.h \
	${UNITTEST}/HashTableTest.cpp \
	${UNITTEST}/LDSymbolTest.h \
	${UNITTEST}/LDSymbolTest.cpp \
	${UNITTEST}/FragmentRefTest.h \
	${UNITTEST}/FragmentRefTest.cpp \
	${UNITTEST}/StaticResolverTest.h \
	${UNITTEST}/StaticResolverTest.cpp \
	${UNITTEST}/GCFactoryListTraitsTest.h \
	${UNITTEST}/GCFactoryListTraitsTest.cpp \
	${UNITTEST}/LEB128Test.h \
	${UNITTEST}/LEB128Test.cpp \
	${UNITTEST}/SymbolCategoryTest.h \
	${UNITTEST}/SymbolCategoryTest.cpp \
	${UNITTEST}/FileHandleTest.h \
	${UNITTEST}/FileHandleTest.cpp \
	${UNITTEST}/FragmentTest.h \
	${UNITTEST}/FragmentTest.cpp \
	${UNITTEST}/SectionDataTest.h \
	${UNITTEST}/SectionDataTest.cpp \
	${UNITTEST}/SystemUtilsTest.h \
	${UNITTEST}/SystemUtilsTest.cpp \
	${UNITTEST}/LinkerTest.h \
	${UNITTEST}/LinkerTest.cpp \
	${UNITTEST}/ELFReaderTest.h \
	${UNITTEST}/ELFReaderTest.cpp \
	${UNITTEST}/ELFBinaryReaderTest.h \
	${UNITTEST}/ELFBinaryReaderTest.cpp
endif

ANDROID_CPPFLAGS=-fno-rtti -fno-exceptions -Waddress -Wchar-subscripts -Wcomment -Wformat -Wparentheses -Wreorder -Wreturn-type -Wsequence-point -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wvolatile-register-var

MCLD_CPPFLAGS = -g -I${top_srcdir}/include -I${top_builddir}/include ${LLVM_CPPFLAGS} ${ANDROID_CPPFLAGS} -I${top_srcdir}/include/mcld/Script

if ENABLE_WERROR
MCLD_CPPFLAGS+=-Werror
endif

bin_PROGRAMS = ld.mcld

if ENABLE_UNITTEST
MCLD_CPPFLAGS += -DTOPDIR=\"${abs_top_srcdir}\" -DENABLE_UNITTEST -DMCLD_DEBUG -I${top_srcdir}/utils/gtest/include -I${top_srcdir}/unittests -DGTEST_HAS_RTTI=0
endif

AM_CPPFLAGS = ${MCLD_CPPFLAGS}

ld_mcld_LDFLAGS = -L${top_builddir}/utils/gtest/lib -lgtest ${LLVM_LDFLAGS}

ld_mcld_LDADD = ${top_builddir}/debug/libmcld.a

dist_ld_mcld_SOURCES = ${MCLD_SOURCES}
