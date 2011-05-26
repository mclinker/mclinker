
TOPLEVEL=..
SRCDIR=${TOPLEVEL}/src
UNITTEST=${TOPLEVEL}/unittests

bin_PROGRAMS = bold

if ENABLE_UNITTEST
bold_CPPFLAGS = -g -O1 -DENABLE_UNITTEST -DDEBUG -I$(srcdir)/../utils/gtest/include -I$(srcdir)/../include -I$(srcdir)/../unittests ${LLVM_CPPFLAGS}
bold_LDADD = -lgtest -L$(srcdir)/../utils/gtest/lib/ ${LLVM_LDFLAGS}
else
bold_CPPFLAGS = -g -O1 -DDEBUG -I$(srcdir)/../utils/gtest/include -I$(srcdir)/../include ${LLVM_CPPFLAGS}
bold_LDADD = ${LLVM_LDFLAGS}
endif

NORMAL_SOURCE = ${SRCDIR}/main.cpp \
	${SRCDIR}/Normal.cpp

if ENABLE_UNITTEST
SOURCE = ${NORMAL_SOURCE} \
	${UNITTEST}/NormalTest.h \
	${UNITTEST}/NormalTest.cpp
else
SOURCE = ${NORMAL_SOURCE}
endif

bold_SOURCES = ${SOURCE}
