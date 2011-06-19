
TOPLEVEL=..
SRCDIR=${TOPLEVEL}/src
LIBDIR=${TOPLEVEL}/lib
INCDIR=${TOPLEVEL}/include/mcld
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
	${LIBDIR}/CodeGen/TargetMachine.cpp \
	${INCDIR}/Target/TargetMachine.h \
	${LIBDIR}/CodeGen/LDPrinter.cpp \
	${INCDIR}/CodeGen/LDPrinter.h

if ENABLE_UNITTEST
SOURCE = ${NORMAL_SOURCE} 
else
SOURCE = ${NORMAL_SOURCE}
endif

bold_SOURCES = ${SOURCE}
