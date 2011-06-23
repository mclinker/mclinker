
TOPLEVEL=..
SRCDIR=${TOPLEVEL}/src
LIBDIR=${TOPLEVEL}/lib
INCDIR=${TOPLEVEL}/include/mcld
UNITTEST=${TOPLEVEL}/unittests

bin_PROGRAMS = MCLinker

if ENABLE_UNITTEST
MCLinker_CPPFLAGS = -g -O1 -DENABLE_UNITTEST -DMCLD_DEBUG -I$(srcdir)/../utils/gtest/include -I$(srcdir)/../include -I$(srcdir)/../unittests ${LLVM_CPPFLAGS}
MCLinker_LDADD = -lgtest -L$(srcdir)/../utils/gtest/lib/ ${LLVM_LDFLAGS}
else
MCLinker_CPPFLAGS = -g -O1 -DMCLD_DEBUG -I$(srcdir)/../utils/gtest/include -I$(srcdir)/../include ${LLVM_CPPFLAGS}
MCLinker_LDADD = ${LLVM_LDFLAGS}
endif

NORMAL_SOURCE = ${SRCDIR}/main.cpp \
	${LIBDIR}/CodeGen/LLVMTargetMachine.cpp \
	${INCDIR}/Target/TargetMachine.h \
	${INCDIR}/Target/TargetRegistry.h \
	${LIBDIR}/Target/Target.cpp
#	${LIBDIR}/CodeGen/LDPrinter.cpp \
#	${INCDIR}/CodeGen/LDPrinter.h

if ENABLE_UNITTEST
SOURCE = ${NORMAL_SOURCE} 
else
SOURCE = ${NORMAL_SOURCE}
endif

MCLinker_SOURCES = ${SOURCE}
