
TOPLEVEL=..
SRCDIR=${TOPLEVEL}/src
LIBDIR=${TOPLEVEL}/lib
INCDIR=${TOPLEVEL}/include/mcld
UNITTEST=${TOPLEVEL}/unittests

bin_PROGRAMS = MCLinker

if ENABLE_UNITTEST
MCLinker_CPPFLAGS = -g -O1 -DLLVM_VERSION=${LLVM_VERSION} -DENABLE_UNITTEST -DMCLD_DEBUG -I$(srcdir)/../utils/gtest/include -I$(srcdir)/../include -I$(srcdir)/../unittests ${LLVM_CPPFLAGS}
MCLinker_LDADD = -lgtest -L$(srcdir)/../utils/gtest/lib/ ${LLVM_LDFLAGS}
else
MCLinker_CPPFLAGS = -g -O1 -DMCLD_DEBUG  -DLLVM_VERSION=${LLVM_VERSION} -I$(srcdir)/../utils/gtest/include -I$(srcdir)/../include ${LLVM_CPPFLAGS}
MCLinker_LDADD = ${LLVM_LDFLAGS}
endif

NORMAL_SOURCE = ${SRCDIR}/main.cpp \
	${LIBDIR}/CodeGen/LLVMTargetMachine.cpp \
	${INCDIR}/Target/TargetMachine.h \
	${INCDIR}/Target/TargetRegistry.h \
	${LIBDIR}/Support/TargetRegistry.cpp \
	${LIBDIR}/Target/Target.cpp \
	${INCDIR}/Target/TargetSelect.h \
	${INCDIR}/Target/TargetLDBackend.h \
	${LIBDIR}/Target/TargetLDBackend.cpp \
	${LIBDIR}/Target/ARM/ARMTargetMachine.cpp \
	${LIBDIR}/Target/ARM/ARMLDBackend.h \
	${LIBDIR}/Target/ARM/ARMLDBackend.cpp \
	${LIBDIR}/Target/ARM/TargetInfo/ARMTargetInfo.cpp \
	${INCDIR}/MC/MCObjectReader.h \
	${LIBDIR}/MC/MCObjectReader.cpp \
	${INCDIR}/MC/MCELFObjectReader.h \
	${LIBDIR}/MC/MCELFObjectReader.cpp \
	${INCDIR}/MC/MCArchiveReader.h \
	${LIBDIR}/MC/MCArchiveReader.cpp \
	${INCDIR}/MC/MCELFArchiveReader.h \
	${LIBDIR}/MC/MCELFArchiveReader.cpp
#        ${UNITTEST}/TargetRegistryTest.h \
#        ${UNITTEST}/TargetRegistryTest.cpp
#	${LIBDIR}/CodeGen/LDPrinter.cpp \
#	${INCDIR}/CodeGen/LDPrinter.h

if ENABLE_UNITTEST
SOURCE = ${NORMAL_SOURCE} 
else
SOURCE = ${NORMAL_SOURCE}
endif

MCLinker_SOURCES = ${SOURCE}
