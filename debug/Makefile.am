BUILDTOP=..
TOPLEVEL=${MCLINKERTOP}
TOOLDIR=${TOPLEVEL}/tools/llvm-mcld
LIBDIR=${TOPLEVEL}/lib
INCDIR=${TOPLEVEL}/include/mcld
UNITTEST=${TOPLEVEL}/unittests

ANDROID_CPPFLAGS=-Werror=format-security -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point

bin_PROGRAMS = llvm-mcld

if ENABLE_UNITTEST
llvm_mcld_CPPFLAGS = -g -O1 -DTOPDIR=\"${TOPLEVEL}\" -DLLVM_VERSION=${LLVM_VERSION} -DENABLE_UNITTEST -DMCLD_DEBUG -I$(TOPLEVEL)/utils/gtest/include -I$(TOPLEVEL)/include -I$(TOPLEVEL)/unittests -I$(BUILDTOP)/include ${LLVM_CPPFLAGS} -DGTEST_HAS_RTTI=0 ${ANDROID_CPPFLAGS}
llvm_mcld_LDADD = -L$(BUILDTOP)/utils/gtest/lib/ ${LLVM_LDFLAGS} -lgtest
else
llvm_mcld_CPPFLAGS = -g -O1 -DLLVM_VERSION=${LLVM_VERSION} -I$(TOPLEVEL)/utils/gtest/include -I$(TOPLEVEL)/include -I$(TOPLEVEL)/include -I$(BUILDTOP)/include ${LLVM_CPPFLAGS} ${ANDROID_CPPFLAGS}
llvm_mcld_LDADD = ${LLVM_LDFLAGS}
endif

NORMAL_SOURCE = ${TOOLDIR}/llvm-mcld.cpp \
	${INCDIR}/ADT/Uncopyable.h \
	${INCDIR}/ADT/TypeTraits.h \
	${INCDIR}/ADT/TreeBase.h \
	${INCDIR}/ADT/Allocators.h \
	${INCDIR}/ADT/TreeAllocator.h \
	${INCDIR}/ADT/BinTree.h \
	${INCDIR}/Support/Path.h \
	${LIBDIR}/Support/Path.cpp \
	${INCDIR}/Support/RealPath.h \
	${LIBDIR}/Support/RealPath.cpp \
	${INCDIR}/Support/Directory.h \
	${LIBDIR}/Support/Directory.cpp \
	${INCDIR}/Support/FileSytem.h \
	${LIBDIR}/Support/FileSystem.cpp \
	${INCDIR}/Support/MemoryArea.h \
	${LIBDIR}/Support/MemoryArea.cpp \
	${INCDIR}/Support/MemoryRegion.h \
	${LIBDIR}/Support/MemoryRegion.cpp \
	${INCDIR}/Support/RegionFactory.h \
	${LIBDIR}/Support/RegionFactory.cpp \
	${INCDIR}/Support/MemoryAreaFactory.h \
	${LIBDIR}/Support/MemoryAreaFactory.cpp \
	${INCDIR}/Support/CommandLine.h \
	${LIBDIR}/Support/CommandLine.cpp \
	${INCDIR}/Support/GCFactory.h \
	${INCDIR}/Support/UniqueGCFactory.h \
	${LIBDIR}/CodeGen/LLVMTargetMachine.cpp \
	${INCDIR}/CodeGen/SectLinker.h \
	${LIBDIR}/CodeGen/SectLinker.cpp \
	${INCDIR}/Target/TargetMachine.h \
	${INCDIR}/Target/TargetRegistry.h \
	${LIBDIR}/Support/TargetRegistry.cpp \
	${LIBDIR}/Target/Target.cpp \
	${INCDIR}/Target/TargetSelect.h \
	${INCDIR}/Target/TargetLDBackend.h \
	${LIBDIR}/Target/TargetLDBackend.cpp \
	${INCDIR}/Target/GNULDBackend.h \
	${LIBDIR}/Target/GNULDBackend.cpp \
	${INCDIR}/Target/AndroidSectLinker.h \
	${LIBDIR}/Target/AndroidSectLinker.cpp \
	${LIBDIR}/Target/ARM/ARMLDBackend.h \
	${LIBDIR}/Target/ARM/ARMLDBackend.cpp \
	${LIBDIR}/Target/ARM/ARMTargetMachine.h \
	${LIBDIR}/Target/ARM/ARMTargetMachine.cpp \
	${LIBDIR}/Target/ARM/ARMRelocationFactory.h \
	${LIBDIR}/Target/ARM/ARMRelocationFactory.cpp \
	${LIBDIR}/Target/ARM/ARMSectLinker.cpp \
	${LIBDIR}/Target/ARM/ARMELFSectLinker.h \
	${LIBDIR}/Target/ARM/ARMELFSectLinker.cpp \
	${LIBDIR}/Target/ARM/ARMAndroidSectLinker.h \
	${LIBDIR}/Target/ARM/ARMAndroidSectLinker.cpp \
	${LIBDIR}/Target/ARM/TargetInfo/ARMTargetInfo.cpp \
	${LIBDIR}/Target/X86/X86LDBackend.h \
	${LIBDIR}/Target/X86/X86LDBackend.cpp \
	${LIBDIR}/Target/X86/X86TargetMachine.h \
	${LIBDIR}/Target/X86/X86TargetMachine.cpp \
	${LIBDIR}/Target/X86/X86RelocationFactory.h \
	${LIBDIR}/Target/X86/X86RelocationFactory.cpp \
	${LIBDIR}/Target/X86/X86SectLinker.cpp \
	${LIBDIR}/Target/X86/X86ELFSectLinker.h \
	${LIBDIR}/Target/X86/X86ELFSectLinker.cpp \
	${LIBDIR}/Target/X86/X86AndroidSectLinker.h \
	${LIBDIR}/Target/X86/X86AndroidSectLinker.cpp \
	${LIBDIR}/Target/X86/TargetInfo/X86TargetInfo.cpp \
	${INCDIR}/MC/MCLDDriver.h \
	${LIBDIR}/MC/MCLDDriver.cpp \
 	${INCDIR}/MC/MCLinker.h \
	${LIBDIR}/MC/MCLinker.cpp \
	${INCDIR}/MC/MCBitcodeInterceptor.h \
	${LIBDIR}/MC/MCBitcodeInterceptor.cpp \
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
	${INCDIR}/MC/ContextFactory.h \
	${LIBDIR}/MC/ContextFactory.cpp \
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
	${INCDIR}/MC/MCRegionFragment.h \
	${LIBDIR}/MC/MCRegionFragment.cpp \
	${INCDIR}/MC/MCFragmentRef.h \
	${LIBDIR}/MC/MCFragmentRef.cpp \
	${INCDIR}/LD/LDContext.h \
	${LIBDIR}/LD/LDContext.cpp \
	${INCDIR}/LD/LDSection.h \
	${INCDIR}/LD/ResolveInfo.h \
	${LIBDIR}/LD/ResolveInfo.cpp \
	${INCDIR}/LD/ResolveInfoFactory.h \
	${LIBDIR}/LD/ResolveInfoFactory.cpp \
	${INCDIR}/LD/Resolver.h \
	${LIBDIR}/LD/Resolver.cpp \
	${INCDIR}/LD/LDSymbol.h \
	${LIBDIR}/LD/LDSymbol.cpp \
	${INCDIR}/LD/Relocation.h \
	${LIBDIR}/LD/Relocation.cpp \
	${INCDIR}/LD/RelocationFactory.h \
	${LIBDIR}/LD/RelocationFactory.cpp \
	${INCDIR}/LD/StaticResolver.h \
	${LIBDIR}/LD/StaticResolver.cpp \
	${INCDIR}/LD/StrSymPool.h \
	${LIBDIR}/LD/StrSymPool.cpp \
	${INCDIR}/LD/LDReader.h \
	${LIBDIR}/LD/LDReader.cpp \
	${INCDIR}/LD/LDWriter.h \
	${LIBDIR}/LD/LDWriter.cpp \
	${INCDIR}/LD/ArchiveReader.h \
	${LIBDIR}/LD/ArchiveReader.cpp \
	${INCDIR}/LD/ObjectReader.h \
	${LIBDIR}/LD/ObjectReader.cpp \
	${INCDIR}/LD/DynObjReader.h \
	${LIBDIR}/LD/DynObjReader.cpp \
	${INCDIR}/LD/ObjectWriter.h \
	${LIBDIR}/LD/ObjectWriter.cpp \
	${INCDIR}/LD/DynObjWriter.h \
	${LIBDIR}/LD/DynObjWriter.cpp \
	${INCDIR}/LD/ELFReader.h \
	${LIBDIR}/LD/ELFReader.cpp \
	${INCDIR}/LD/ELFWriter.h \
	${LIBDIR}/LD/ELFWriter.cpp \
	${INCDIR}/LD/GNUArchiveReader.h \
	${LIBDIR}/LD/GNUArchiveReader.cpp \
	${INCDIR}/LD/BSDArchiveReader.h \
	${LIBDIR}/LD/BSDArchiveReader.cpp \
	${INCDIR}/LD/ELFObjectReader.h \
	${LIBDIR}/LD/ELFObjectReader.cpp \
	${INCDIR}/LD/ELFDynObjReader.h \
	${LIBDIR}/LD/ELFDynObjReader.cpp \
	${INCDIR}/LD/ELFObjectWriter.h \
	${LIBDIR}/LD/ELFObjectWriter.cpp \
	${INCDIR}/LD/ELFDynObjWriter.h \
	${LIBDIR}/LD/ELFDynObjWriter.cpp
#	${LIBDIR}/CodeGen/LDPrinter.cpp \
#	${INCDIR}/CodeGen/LDPrinter.h
#	${INCDIR}/Support/ScopedRegion.h \
#	${LIBDIR}/Support/ScopedRegion.cpp

if ENABLE_UNITTEST
SOURCE = ${NORMAL_SOURCE} \
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
        ${UNITTEST}/MCFragmentRefTest.h \
        ${UNITTEST}/MCFragmentRefTest.cpp
#	${UNITTEST}/SymbolTableTest.h \
#	${UNITTEST}/SymbolTableTest.cpp \
#        ${UNITTEST}/TargetRegistryTest.h \
#        ${UNITTEST}/TargetRegistryTest.cpp
else
SOURCE = ${NORMAL_SOURCE}
endif

llvm_mcld_SOURCES = ${SOURCE}
