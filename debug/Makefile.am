
TOPLEVEL=..
SRCDIR=${TOPLEVEL}/src
LIBDIR=${TOPLEVEL}/lib
INCDIR=${TOPLEVEL}/include/mcld
UNITTEST=${TOPLEVEL}/unittests

bin_PROGRAMS = MCLinker

if ENABLE_UNITTEST
MCLinker_CPPFLAGS = -g -O1 -DLLVM_VERSION=${LLVM_VERSION} -DENABLE_UNITTEST -DMCLD_DEBUG -I$(srcdir)/../utils/gtest/include -I$(srcdir)/../include -I$(srcdir)/../unittests ${LLVM_CPPFLAGS} -DGTEST_HAS_RTTI=0
MCLinker_LDADD = -lgtest -L$(srcdir)/../utils/gtest/lib/ ${LLVM_LDFLAGS}
else
MCLinker_CPPFLAGS = -g -O1 -DLLVM_VERSION=${LLVM_VERSION} -I$(srcdir)/../utils/gtest/include -I$(srcdir)/../include ${LLVM_CPPFLAGS}
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
	${INCDIR}/Target/GNULDBackend.h \
	${LIBDIR}/Target/GNULDBackend.cpp \
	${INCDIR}/MC/MCObjectReader.h \
	${LIBDIR}/MC/MCObjectReader.cpp \
	${INCDIR}/MC/MCELFObjectReader.h \
	${LIBDIR}/MC/MCELFObjectReader.cpp \
	${INCDIR}/MC/MCELFObjectTargetReader.h \
	${LIBDIR}/MC/MCELFObjectTargetReader.cpp \
	${INCDIR}/MC/MCObjectWriter.h \
	${LIBDIR}/MC/MCObjectWriter.cpp \
	${INCDIR}/MC/MCELFObjectWriter.h \
	${LIBDIR}/MC/MCELFObjectWriter.cpp \
	${INCDIR}/MC/MCELFObjectTargetWriter.h \
	${LIBDIR}/MC/MCELFObjectTargetWriter.cpp \
	${INCDIR}/MC/MCArchiveReader.h \
	${LIBDIR}/MC/MCArchiveReader.cpp \
	${INCDIR}/MC/MCGNUArchiveReader.h \
	${LIBDIR}/MC/MCGNUArchiveReader.cpp \
	${INCDIR}/CodeGen/SectLinker.h \
	${LIBDIR}/CodeGen/SectLinker.cpp \
	${LIBDIR}/Target/ARM/ARMLDBackend.h \
	${LIBDIR}/Target/ARM/ARMLDBackend.cpp \
	${LIBDIR}/Target/ARM/ARMTargetMachine.h \
	${LIBDIR}/Target/ARM/ARMTargetMachine.cpp \
	${LIBDIR}/Target/ARM/ARMELFObjectReader.h \
	${LIBDIR}/Target/ARM/ARMELFObjectReader.cpp \
	${LIBDIR}/Target/ARM/ARMELFObjectWriter.h \
	${LIBDIR}/Target/ARM/ARMELFObjectWriter.cpp \
	${LIBDIR}/Target/ARM/ARMSectLinker.cpp \
	${LIBDIR}/Target/ARM/ARMELFSectLinker.h \
	${LIBDIR}/Target/ARM/ARMELFSectLinker.cpp \
	${LIBDIR}/Target/ARM/TargetInfo/ARMTargetInfo.cpp \
	${LIBDIR}/Target/X86/X86LDBackend.h \
	${LIBDIR}/Target/X86/X86LDBackend.cpp \
	${LIBDIR}/Target/X86/X86TargetMachine.h \
	${LIBDIR}/Target/X86/X86TargetMachine.cpp \
	${LIBDIR}/Target/X86/X86ELFObjectReader.h \
	${LIBDIR}/Target/X86/X86ELFObjectReader.cpp \
	${LIBDIR}/Target/X86/X86ELFObjectWriter.h \
	${LIBDIR}/Target/X86/X86ELFObjectWriter.cpp \
	${LIBDIR}/Target/X86/X86SectLinker.cpp \
	${LIBDIR}/Target/X86/X86ELFSectLinker.h \
	${LIBDIR}/Target/X86/X86ELFSectLinker.cpp \
	${LIBDIR}/Target/X86/TargetInfo/X86TargetInfo.cpp \
	${INCDIR}/MC/MCLDDriver.h \
	${LIBDIR}/MC/MCLDDriver.cpp \
 	${INCDIR}/MC/MCLinker.h \
	${LIBDIR}/MC/MCLinker.cpp \
	${INCDIR}/MC/MCAsmObjectReader.h \
	${LIBDIR}/MC/MCAsmObjectReader.cpp \
	${INCDIR}/MC/MCLDFile.h \
	${LIBDIR}/MC/MCLDFile.cpp \
	${INCDIR}/MC/MCLDInput.h \
	${LIBDIR}/MC/MCLDInput.cpp \
	${INCDIR}/MC/MCLDOutput.h \
	${LIBDIR}/MC/MCLDOutput.cpp \
	${INCDIR}/MC/InputFactory.h \
	${LIBDIR}/MC/InputFactory.cpp \
	${INCDIR}/MC/MCLDOptions.h \
	${LIBDIR}/MC/MCLDOptions.cpp \
	${INCDIR}/MC/MCLDInfo.h \
	${LIBDIR}/MC/MCLDInfo.cpp \
	${INCDIR}/MC/MCLDContext.h \
	${LIBDIR}/MC/MCLDContext.cpp \
	${INCDIR}/MC/ContextFactory.h \
	${LIBDIR}/MC/ContextFactory.cpp \
	${INCDIR}/Support/Path.h \
	${LIBDIR}/Support/Path.cpp \
	${INCDIR}/Support/RealPath.h \
	${LIBDIR}/Support/RealPath.cpp \
	${INCDIR}/Support/Directory.h \
	${LIBDIR}/Support/Directory.cpp \
	${INCDIR}/Support/FileSytem.h \
	${LIBDIR}/Support/FileSystem.cpp \
	${INCDIR}/MC/SearchDirs.h \
	${LIBDIR}/MC/SearchDirs.cpp \
	${INCDIR}/MC/MCLDDirectory.h \
	${LIBDIR}/MC/MCLDDirectory.cpp \
	${INCDIR}/MC/MCLDAttribute.h \
	${LIBDIR}/MC/MCLDAttribute.cpp \
	${INCDIR}/MC/AttributeFactory.h \
	${LIBDIR}/MC/AttributeFactory.cpp \
	${INCDIR}/MC/MCLDInputTree.h \
	${LIBDIR}/MC/MCLDInputTree.cpp \
	${INCDIR}/Support/CommandLine.h \
	${LIBDIR}/Support/CommandLine.cpp \
	${INCDIR}/ADT/Uncopyable.h \
	${INCDIR}/ADT/TypeTraits.h \
	${INCDIR}/ADT/TreeBase.h \
	${INCDIR}/ADT/Allocators.h \
	${INCDIR}/ADT/GCFactory.h \
	${INCDIR}/ADT/TreeAllocator.h \
	${INCDIR}/ADT/BinTree.h \
	${INCDIR}/ADT/StringMap.h
#	${LIBDIR}/CodeGen/LDPrinter.cpp \
#	${INCDIR}/CodeGen/LDPrinter.h

if ENABLE_UNITTEST
SOURCE = ${NORMAL_SOURCE} \
	${UNITTEST}/StringMapTest.h \
	${UNITTEST}/StringMapTest.cpp \
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
	${UNITTEST}/DirIteratorTest.cpp
#        ${UNITTEST}/TargetRegistryTest.h \
#        ${UNITTEST}/TargetRegistryTest.cpp
else
SOURCE = ${NORMAL_SOURCE}
endif

MCLinker_SOURCES = ${SOURCE}
