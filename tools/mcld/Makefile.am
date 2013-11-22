MCLD_SOURCES = ${srcdir}/main.cpp

if ENABLE_UNITTEST
UNITTEST=${top_srcdir}/unittests

MCLD_SOURCES += \
	${UNITTEST}/BinTreeTest.cpp \
	${UNITTEST}/BinTreeTest.h \
	${UNITTEST}/DirIteratorTest.cpp \
	${UNITTEST}/DirIteratorTest.h \
	${UNITTEST}/ELFBinaryReaderTest.cpp \
	${UNITTEST}/ELFBinaryReaderTest.h \
	${UNITTEST}/ELFReaderTest.cpp \
	${UNITTEST}/ELFReaderTest.h \
	${UNITTEST}/FileHandleTest.cpp \
	${UNITTEST}/FileHandleTest.h \
	${UNITTEST}/FragmentRefTest.cpp \
	${UNITTEST}/FragmentRefTest.h \
	${UNITTEST}/FragmentTest.cpp \
	${UNITTEST}/FragmentTest.h \
	${UNITTEST}/GCFactoryListTraitsTest.cpp \
	${UNITTEST}/GCFactoryListTraitsTest.h \
	${UNITTEST}/HashTableTest.cpp \
	${UNITTEST}/HashTableTest.h \
	${UNITTEST}/InputTreeTest.cpp \
	${UNITTEST}/InputTreeTest.h \
	${UNITTEST}/LDSymbolTest.cpp \
	${UNITTEST}/LDSymbolTest.h \
	${UNITTEST}/LEB128Test.cpp \
	${UNITTEST}/LEB128Test.h \
	${UNITTEST}/LinearAllocatorTest.cpp \
	${UNITTEST}/LinearAllocatorTest.h \
	${UNITTEST}/LinkerTest.cpp \
	${UNITTEST}/LinkerTest.h \
	${UNITTEST}/PathTest.cpp \
	${UNITTEST}/PathTest.h \
	${UNITTEST}/RTLinearAllocatorTest.h \
	${UNITTEST}/RTLinearAllocatorTest.cpp \
	${UNITTEST}/SectionDataTest.cpp \
	${UNITTEST}/SectionDataTest.h \
	${UNITTEST}/StaticResolverTest.cpp \
	${UNITTEST}/StaticResolverTest.h \
	${UNITTEST}/SymbolCategoryTest.cpp \
	${UNITTEST}/SymbolCategoryTest.h \
	${UNITTEST}/SystemUtilsTest.cpp \
	${UNITTEST}/SystemUtilsTest.h \
	${UNITTEST}/UniqueGCFactoryBaseTest.cpp \
	${UNITTEST}/UniqueGCFactoryBaseTest.h
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

if ENABLE_UNITTEST
ld_mcld_LDFLAGS = -L${top_builddir}/utils/gtest/lib -lgtest ${LLVM_LDFLAGS}
ld_mcld_LDADD = ${top_builddir}/debug/libmcld.a
else
ld_mcld_LDFLAGS = ${LLVM_LDFLAGS}
ld_mcld_LDADD = ${top_builddir}/optimized/libmcld.a
endif

dist_ld_mcld_SOURCES = ${MCLD_SOURCES}
