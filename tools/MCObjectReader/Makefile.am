TOPLEVEL = ../../
SRCDIR = ${TOPLEVEL}/tools/MCObjectReader
LIBDIR = ${TOPLEVEL}/lib
INCDIR = ${TOLEVEL}/include/mcld
UNITTEST = ${TOPLEVEL}/unittests

bin_PROGRAMS = MCObjectReader

if ENABLE_UNITTEST
MCObjectReader_CPPFLAGS = -g -O1 -DLLVM_VERSION=${LLVM_VERSION} -DENABLE_UNITTEST -DOBJECTREADER_DEBUG -I$(srcdir)/../../utils/gtest/include -I$(srcdir)/../../include -I$(srcdir)/../../unittests ${LLVM_CPPFLAGS}

MCObjectReader_LDADD = -lgtest -L$(srcdir)/../../utils/gtest/lib ${LLVM_LDFLAGS}
else
MCObjectReader_CPPFLAGS = -g -O1 -DOBJECTREADER_DEBUG -DLLVM_VERSION=${LLVM_VERSION} -I$(srcdir)/../../utils/gtest/include -I$(srcdir)/../../include ${LLVM_CPPFLAGS}
MCObjectReader_LDADD = ${LLVM_LDFLAGS}
endif

NORMAL_SOURCE = \
	${SRCDIR}/main.cpp \
	$(LIBDIR)/MC/MCELFObjectReader.cpp \
	$(LIBDIR)/MC/MCELFObjectTargetReader.cpp \
	$(LIBDIR)/MC/MCLDContext.cpp \
	$(LIBDIR)/MC/MCLDFile.cpp \
	$(LIBDIR)/MC/MCObjectReader.cpp \
	$(LIBDIR)/Support/Path.cpp

if ENABLE_UNITTEST
SOURCE = ${NORMAL_SOURCE}
else
SOURCE = ${NORMAL_SOURCE}
endif

MCObjectReader_SOURCES = ${SOURCE}

