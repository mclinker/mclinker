BUILDTOP=..
TOPLEVEL=${srcdir}/..
TOOLDIR=${TOPLEVEL}/tools
LIBDIR=${TOPLEVEL}/lib
INCDIR=${TOPLEVEL}/include/mcld
UNITTEST=${TOPLEVEL}/unittests

SOURCE = ${INCDIR}/ADT/Uncopyable.h \
	${INCDIR}/ADT/TypeTraits.h \
	${INCDIR}/ADT/SizeTraits.h \
	${INCDIR}/ADT/TreeBase.h \
	${INCDIR}/ADT/TreeAllocator.h \
	${INCDIR}/ADT/BinTree.h \
	${INCDIR}/ADT/StringEntry.h \
	${INCDIR}/ADT/StringEntry.tcc \
	${LIBDIR}/ADT/StringEntry.cpp \
	${INCDIR}/ADT/HashEntry.h \
	${INCDIR}/ADT/HashEntry.tcc \
	${INCDIR}/ADT/HashEntryFactory.h \
	${INCDIR}/ADT/HashIterator.h \
	${INCDIR}/ADT/HashBase.h \
	${INCDIR}/ADT/HashBase.tcc \
	${INCDIR}/ADT/HashTable.h \
	${INCDIR}/ADT/HashTable.tcc \
	${INCDIR}/ADT/StringHash.h \
	${INCDIR}/ADT/Flags.h \
	${INCDIR}/Support/Allocators.h \
	${INCDIR}/Support/Path.h \
	${LIBDIR}/Support/Path.cpp \
	${INCDIR}/Support/PathCache.h \
	${INCDIR}/Support/RealPath.h \
	${LIBDIR}/Support/RealPath.cpp \
	${INCDIR}/Support/Directory.h \
	${LIBDIR}/Support/Directory.cpp \
	${INCDIR}/Support/FileSystem.h \
	${LIBDIR}/Support/FileSystem.cpp \
	${INCDIR}/Support/SystemUtils.h \
	${LIBDIR}/Support/SystemUtils.cpp \
	${INCDIR}/Support/LEB128.h \
	${LIBDIR}/Support/LEB128.cpp \
	${INCDIR}/Support/FileHandle.h \
	${LIBDIR}/Support/FileHandle.cpp \
	${INCDIR}/Support/MemoryArea.h \
	${LIBDIR}/Support/MemoryArea.cpp \
	${INCDIR}/Support/Space.h \
	${LIBDIR}/Support/Space.cpp \
	${INCDIR}/Support/MemoryRegion.h \
	${LIBDIR}/Support/MemoryRegion.cpp \
	${INCDIR}/Support/RegionFactory.h \
	${LIBDIR}/Support/RegionFactory.cpp \
	${INCDIR}/Support/HandleToArea.h \
	${LIBDIR}/Support/HandleToArea.cpp \
	${INCDIR}/Support/MemoryAreaFactory.h \
	${LIBDIR}/Support/MemoryAreaFactory.cpp \
	${INCDIR}/Support/raw_mem_ostream.h \
	${LIBDIR}/Support/raw_mem_ostream.cpp \
	${INCDIR}/Support/ToolOutputFile.h\
	${LIBDIR}/Support/ToolOutputFile.cpp \
	${INCDIR}/Support/CommandLine.h \
	${LIBDIR}/Support/CommandLine.cpp \
	${INCDIR}/Support/GCFactory.h \
	${INCDIR}/Support/UniqueGCFactory.h \
	${INCDIR}/Support/GCFactoryListTraits.h \
	${INCDIR}/Support/TargetRegistry.h \
	${LIBDIR}/Support/TargetRegistry.cpp \
	${INCDIR}/Support/TargetSelect.h \
	${INCDIR}/Support/raw_ostream.h \
	${LIBDIR}/Support/raw_ostream.cpp \
	${INCDIR}/Support/MsgHandling.h \
	${LIBDIR}/Support/MsgHandling.cpp \
	${LIBDIR}/Support/Unix/FileSystem.inc \
	${LIBDIR}/Support/Unix/PathV3.inc \
	${LIBDIR}/Support/Unix/System.inc \
	${LIBDIR}/Support/Windows/FileSystem.inc \
	${LIBDIR}/Support/Windows/PathV3.inc \
	${LIBDIR}/Support/Windows/System.inc \
	${LIBDIR}/CodeGen/LLVMTargetMachine.cpp \
	${INCDIR}/CodeGen/MCLinker.h \
	${LIBDIR}/CodeGen/MCLinker.cpp \
	${INCDIR}/Target/SymbolEntryMap.h \
	${INCDIR}/Target/PLT.h\
	${LIBDIR}/Target/PLT.cpp\
	${INCDIR}/Target/GOT.h \
	${LIBDIR}/Target/GOT.cpp \
	${INCDIR}/Target/TargetMachine.h \
	${LIBDIR}/Target/Target.cpp \
	${INCDIR}/Target/TargetLDBackend.h \
	${LIBDIR}/Target/TargetLDBackend.cpp \
	${INCDIR}/Target/GNULDBackend.h \
	${LIBDIR}/Target/GNULDBackend.cpp \
	${INCDIR}/Target/ELFMCLinker.h \
	${LIBDIR}/Target/ELFMCLinker.cpp \
	${INCDIR}/Target/ELFDynamic.h \
	${INCDIR}/Target/ELFDynamic.tcc \
	${LIBDIR}/Target/ELFDynamic.cpp \
	${INCDIR}/Target/OutputRelocSection.h \
	${LIBDIR}/Target/OutputRelocSection.cpp \
	${LIBDIR}/Target/ARM/ARMELFDynamic.h \
	${LIBDIR}/Target/ARM/ARMELFDynamic.cpp \
	${LIBDIR}/Target/ARM/ARMLDBackend.h \
	${LIBDIR}/Target/ARM/ARMLDBackend.cpp \
	${LIBDIR}/Target/ARM/ARMGOT.h \
	${LIBDIR}/Target/ARM/ARMGOT.cpp \
	${LIBDIR}/Target/ARM/ARMPLT.h \
	${LIBDIR}/Target/ARM/ARMPLT.cpp \
	${LIBDIR}/Target/ARM/ARMTargetMachine.h \
	${LIBDIR}/Target/ARM/ARMTargetMachine.cpp \
	${LIBDIR}/Target/ARM/ARMRelocationFactory.h \
	${LIBDIR}/Target/ARM/ARMRelocationFactory.cpp \
	${LIBDIR}/Target/ARM/ARMMCLinker.cpp \
	${LIBDIR}/Target/ARM/ARMELFMCLinker.h \
	${LIBDIR}/Target/ARM/ARMELFMCLinker.cpp \
	${LIBDIR}/Target/ARM/ARMDiagnostic.cpp \
	${LIBDIR}/Target/ARM/TargetInfo/ARMTargetInfo.cpp \
	${LIBDIR}/Target/Mips/MipsELFDynamic.h \
	${LIBDIR}/Target/Mips/MipsELFDynamic.cpp \
	${LIBDIR}/Target/Mips/MipsLDBackend.h \
	${LIBDIR}/Target/Mips/MipsLDBackend.cpp \
	${LIBDIR}/Target/Mips/MipsGOT.h \
	${LIBDIR}/Target/Mips/MipsGOT.cpp \
	${LIBDIR}/Target/Mips/MipsTargetMachine.h \
	${LIBDIR}/Target/Mips/MipsTargetMachine.cpp \
	${LIBDIR}/Target/Mips/MipsRelocationFactory.h \
	${LIBDIR}/Target/Mips/MipsRelocationFactory.cpp \
	${LIBDIR}/Target/Mips/MipsMCLinker.cpp \
	${LIBDIR}/Target/Mips/MipsELFMCLinker.h \
	${LIBDIR}/Target/Mips/MipsELFMCLinker.cpp \
	${LIBDIR}/Target/Mips/MipsDiagnostic.cpp \
	${LIBDIR}/Target/Mips/TargetInfo/MipsTargetInfo.cpp \
	${LIBDIR}/Target/X86/X86ELFDynamic.h \
	${LIBDIR}/Target/X86/X86ELFDynamic.cpp \
	${LIBDIR}/Target/X86/X86LDBackend.h \
	${LIBDIR}/Target/X86/X86LDBackend.cpp \
	${LIBDIR}/Target/X86/X86GOT.h \
	${LIBDIR}/Target/X86/X86GOT.cpp \
	${LIBDIR}/Target/X86/X86GOTPLT.h \
	${LIBDIR}/Target/X86/X86GOTPLT.cpp \
	${LIBDIR}/Target/X86/X86PLT.h \
	${LIBDIR}/Target/X86/X86PLT.cpp \
	${LIBDIR}/Target/X86/X86TargetMachine.h \
	${LIBDIR}/Target/X86/X86TargetMachine.cpp \
	${LIBDIR}/Target/X86/X86RelocationFactory.h \
	${LIBDIR}/Target/X86/X86RelocationFactory.cpp \
	${LIBDIR}/Target/X86/X86MCLinker.cpp \
	${LIBDIR}/Target/X86/X86ELFMCLinker.h \
	${LIBDIR}/Target/X86/X86ELFMCLinker.cpp \
	${LIBDIR}/Target/X86/X86Diagnostic.cpp \
	${LIBDIR}/Target/X86/TargetInfo/X86TargetInfo.cpp \
	${INCDIR}/Object/ObjectLinker.h \
	${LIBDIR}/Object/ObjectLinker.cpp \
	${INCDIR}/MC/SymbolCategory.h \
	${LIBDIR}/MC/SymbolCategory.cpp \
	${INCDIR}/Fragment/FragmentLinker.h \
	${INCDIR}/Fragment/FragmentLinker.tcc \
	${LIBDIR}/Fragment/FragmentLinker.cpp \
	${INCDIR}/Fragment/Fragment.h \
	${LIBDIR}/Fragment/Fragment.cpp \
	${INCDIR}/Fragment/FragmentRef.h \
	${LIBDIR}/Fragment/FragmentRef.cpp \
	${INCDIR}/Fragment/AlignFragment.h \
	${LIBDIR}/Fragment/AlignFragment.cpp \
	${INCDIR}/Fragment/RegionFragment.h \
	${LIBDIR}/Fragment/RegionFragment.cpp \
	${INCDIR}/Fragment/FillFragment.h \
	${LIBDIR}/Fragment/FillFragment.cpp \
	${INCDIR}/Fragment/TargetFragment.h \
	${INCDIR}/Fragment/Relocation.h \
	${LIBDIR}/Fragment/Relocation.cpp \
	${INCDIR}/Fragment/Stub.h \
	${LIBDIR}/Fragment/Stub.cpp \
	${INCDIR}/MC/MCLDInput.h \
	${LIBDIR}/MC/MCLDInput.cpp \
	${INCDIR}/MC/InputFactory.h \
	${LIBDIR}/MC/InputFactory.cpp \
	${INCDIR}/Module.h \
	${LIBDIR}/Core/Module.cpp \
	${INCDIR}/LinkerConfig.h \
	${LIBDIR}/Core/LinkerConfig.cpp \
	${INCDIR}/GeneralOptions.h \
	${LIBDIR}/Core/GeneralOptions.cpp \
	${INCDIR}/ScriptOptions.h \
	${LIBDIR}/Core/ScriptOptions.cpp \
	${INCDIR}/BitcodeOption.h \
	${LIBDIR}/Core/BitcodeOption.cpp \
	${INCDIR}/AttributeOption.h \
	${LIBDIR}/Core/AttributeOption.cpp \
	${INCDIR}/MC/ContextFactory.h \
	${LIBDIR}/MC/ContextFactory.cpp \
	${INCDIR}/MC/SearchDirs.h \
	${LIBDIR}/MC/SearchDirs.cpp \
	${INCDIR}/MC/MCLDDirectory.h \
	${LIBDIR}/MC/MCLDDirectory.cpp \
	${INCDIR}/MC/Attribute.h \
	${LIBDIR}/MC/Attribute.cpp \
	${INCDIR}/MC/AttributeSet.h \
	${LIBDIR}/MC/AttributeSet.cpp \
	${INCDIR}/MC/InputTree.h \
	${LIBDIR}/MC/InputTree.cpp \
	${INCDIR}/MC/ZOption.h \
	${LIBDIR}/MC/ZOption.cpp \
	${INCDIR}/MC/InputAction.h \
	${LIBDIR}/MC/InputAction.cpp \
	${INCDIR}/MC/CommandAction.h \
	${LIBDIR}/MC/CommandAction.cpp \
	${INCDIR}/MC/FileAction.h \
	${LIBDIR}/MC/FileAction.cpp \
	${INCDIR}/MC/InputBuilder.h \
	${LIBDIR}/MC/InputBuilder.cpp \
	${INCDIR}/LD/DiagCommonKinds.inc \
	${INCDIR}/LD/DiagReaders.inc \
	${INCDIR}/LD/DiagSymbolResolutions.inc \
	${INCDIR}/LD/DiagRelocations.inc \
	${INCDIR}/LD/DiagLayouts.inc \
	${INCDIR}/LD/DiagGOTPLT.inc \
	${INCDIR}/LD/LDContext.h \
	${LIBDIR}/LD/LDContext.cpp \
	${INCDIR}/LD/LDSection.h \
	${LIBDIR}/LD/LDSection.cpp \
	${INCDIR}/LD/SectionData.h \
	${LIBDIR}/LD/SectionData.cpp \
	${INCDIR}/LD/ResolveInfo.h \
	${LIBDIR}/LD/ResolveInfo.cpp \
	${INCDIR}/LD/Resolver.h \
	${LIBDIR}/LD/Resolver.cpp \
	${INCDIR}/LD/LDSymbol.h \
	${LIBDIR}/LD/LDSymbol.cpp \
	${INCDIR}/LD/Layout.h \
	${LIBDIR}/LD/Layout.cpp\
	${INCDIR}/LD/RelocationFactory.h \
	${LIBDIR}/LD/RelocationFactory.cpp \
	${INCDIR}/LD/StaticResolver.h \
	${LIBDIR}/LD/StaticResolver.cpp \
	${INCDIR}/LD/NamePool.h \
	${LIBDIR}/LD/NamePool.cpp \
	${INCDIR}/LD/LDReader.h \
	${LIBDIR}/LD/LDReader.cpp \
	${INCDIR}/LD/LDWriter.h \
	${LIBDIR}/LD/LDWriter.cpp \
	${INCDIR}/LD/ArchiveReader.h \
	${LIBDIR}/LD/ArchiveReader.cpp \
	${INCDIR}/LD/ObjectReader.h \
	${INCDIR}/LD/DynObjReader.h \
	${LIBDIR}/LD/DynObjReader.cpp \
	${INCDIR}/LD/GroupReader.h \
	${LIBDIR}/LD/GroupReader.cpp \
	${INCDIR}/LD/ObjectWriter.h \
	${LIBDIR}/LD/ObjectWriter.cpp \
	${INCDIR}/LD/DynObjWriter.h \
	${LIBDIR}/LD/DynObjWriter.cpp \
	${INCDIR}/LD/ExecWriter.h \
	${LIBDIR}/LD/ExecWriter.cpp \
	${INCDIR}/LD/ELFReader.h \
	${INCDIR}/LD/ELFReader.tcc \
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
	${LIBDIR}/LD/ELFDynObjWriter.cpp \
	${INCDIR}/LD/ELFExecWriter.h \
	${LIBDIR}/LD/ELFExecWriter.cpp \
	${INCDIR}/LD/LDFileFormat.h \
	${LIBDIR}/LD/LDFileFormat.cpp \
	${INCDIR}/LD/ELFFileFormat.h \
	${LIBDIR}/LD/ELFFileFormat.cpp \
	${INCDIR}/LD/ELFDynObjFileFormat.h \
	${LIBDIR}/LD/ELFDynObjFileFormat.cpp \
	${INCDIR}/LD/ELFExecFileFormat.h \
	${LIBDIR}/LD/ELFExecFileFormat.cpp \
	${INCDIR}/LD/SectionMap.h \
	${LIBDIR}/LD/SectionMap.cpp \
	${INCDIR}/LD/SectionMerger.h \
	${LIBDIR}/LD/SectionMerger.cpp \
	${INCDIR}/LD/ELFSegment.h \
	${LIBDIR}/LD/ELFSegment.cpp \
	${INCDIR}/LD/ELFSegmentFactory.h \
	${LIBDIR}/LD/ELFSegmentFactory.cpp \
	${INCDIR}/LD/EhFrame.h \
	${LIBDIR}/LD/EhFrame.cpp \
	${INCDIR}/LD/EhFrameHdr.h \
	${INCDIR}/LD/EhFrameHdr.tcc \
	${LIBDIR}/LD/EhFrameHdr.cpp \
	${INCDIR}/LD/CIE.h \
	${LIBDIR}/LD/CIE.cpp \
	${INCDIR}/LD/FDE.h \
	${LIBDIR}/LD/FDE.cpp \
	${INCDIR}/LD/MsgHandler.h \
	${LIBDIR}/LD/MsgHandler.cpp \
	${INCDIR}/LD/Diagnostic.h \
	${LIBDIR}/LD/Diagnostic.cpp \
	${INCDIR}/LD/DiagnosticInfos.h \
	${LIBDIR}/LD/DiagnosticInfos.cpp \
	${INCDIR}/LD/DiagnosticLineInfo.h \
	${LIBDIR}/LD/DiagnosticLineInfo.cpp \
	${INCDIR}/LD/DWARFLineInfo.h \
	${LIBDIR}/LD/DWARFLineInfo.cpp \
	${INCDIR}/LD/DiagnosticEngine.h \
	${LIBDIR}/LD/DiagnosticEngine.cpp \
	${INCDIR}/LD/DiagnosticPrinter.h \
	${LIBDIR}/LD/DiagnosticPrinter.cpp \
	${INCDIR}/LD/TextDiagnosticPrinter.h \
	${LIBDIR}/LD/TextDiagnosticPrinter.cpp \
	${INCDIR}/LD/Archive.h \
	${LIBDIR}/LD/Archive.cpp \
	${INCDIR}/LD/BranchIsland.h \
	${LIBDIR}/LD/BranchIsland.cpp

if ENABLE_UNITTEST
UNITTEST_SOURCE = \
	${UNITTEST}/Linker/TestLinker.h \
	${UNITTEST}/Linker/TestLinker.cpp \
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
	${UNITTEST}/TestLinkerTest.h \
	${UNITTEST}/TestLinkerTest.cpp\
	${UNITTEST}/FragmentTest.h \
	${UNITTEST}/FragmentTest.cpp\
	${UNITTEST}/SectionDataTest.h \
	${UNITTEST}/SectionDataTest.cpp \
	${UNITTEST}/ReadStageTest.h \
	${UNITTEST}/ReadStageTest.cpp \
	${UNITTEST}/SystemUtilsTest.h \
	${UNITTEST}/SystemUtilsTest.cpp

else
UNITTEST_SOURCE =
endif

ANDROID_CPPFLAGS=-Wformat -Werror=format-security -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point

noinst_LTLIBRARIES = ../lib/libmcld.debug.la

___lib_libmcld_debug_la_SOURCES = ${SOURCE}

bin_PROGRAMS = llvm-mcld.debug mcld.debug

if ENABLE_UNITTEST
AM_CPPFLAGS = -g -O1 -DTOPDIR=\"${abs_top_srcdir}\" -DLLVM_VERSION=${LLVM_VERSION} -DENABLE_UNITTEST -DMCLD_DEBUG -I$(TOPLEVEL)/utils/gtest/include -I$(TOPLEVEL)/include -I$(TOPLEVEL)/unittests -I$(BUILDTOP)/include ${LLVM_CPPFLAGS} -DGTEST_HAS_RTTI=0 ${ANDROID_CPPFLAGS} -I$(TOOLDIR)/mcld/include -I$(BUILDTOP)/tools/mcld/include

llvm_mcld_debug_CPPFLAGS = -g -O1 -DTOPDIR=\"${abs_top_srcdir}\" -DLLVM_VERSION=${LLVM_VERSION} -DENABLE_UNITTEST -DMCLD_DEBUG -I$(TOPLEVEL)/utils/gtest/include -I$(TOPLEVEL)/include -I$(TOPLEVEL)/unittests -I$(BUILDTOP)/include ${LLVM_CPPFLAGS} -DGTEST_HAS_RTTI=0 ${ANDROID_CPPFLAGS}

llvm_mcld_debug_LDFLAGS = -L$(BUILDTOP)/utils/gtest/lib -lgtest ${LLVM_LDFLAGS} -L$(BUILDTOP)/lib
llvm_mcld_debug_LDADD = ../lib/libmcld.debug.la

mcld_debug_CPPFLAGS = -g -O1 -DTOPDIR=\"${abs_top_srcdir}\" -DLLVM_VERSION=${LLVM_VERSION} -DENABLE_UNITTEST -DMCLD_DEBUG -I$(TOPLEVEL)/utils/gtest/include -I$(TOPLEVEL)/include -I$(TOPLEVEL)/unittests -I$(BUILDTOP)/include ${LLVM_CPPFLAGS} -DGTEST_HAS_RTTI=0 ${ANDROID_CPPFLAGS} -I$(TOPLEVEL)/tools/mcld/include -I$(BUILDTOP)/tools/mcld/include

mcld_debug_LDFLAGS = -L$(BUILDTOP)/utils/gtest/lib -lgtest ${LLVM_LDFLAGS} -L$(BUILDTOP)/lib
mcld_debug_LDADD = ../lib/libmcld.debug.la
else
AM_CPPFLAGS = -g -O1 -DLLVM_VERSION=${LLVM_VERSION} -I$(TOPLEVEL)/include -I$(TOPLEVEL)/include -I$(BUILDTOP)/include ${LLVM_CPPFLAGS} ${ANDROID_CPPFLAGS}

llvm_mcld_debug_CPPFLAGS = -g -O1 -DLLVM_VERSION=${LLVM_VERSION} -I$(TOPLEVEL)/include -I$(BUILDTOP)/include ${LLVM_CPPFLAGS} ${ANDROID_CPPFLAGS}

llvm_mcld_debug_LDFLAGS = ${LLVM_LDFLAGS} -L$(BUILDTOP)/lib
llvm_mcld_debug_LDADD = ../lib/libmcld.debug.la

mcld_debug_CPPFLAGS = -g -O1 -DLLVM_VERSION=${LLVM_VERSION} -I$(TOPLEVEL)/include -I$(BUILDTOP)/include ${LLVM_CPPFLAGS} ${ANDROID_CPPFLAGS} -I$(TOPLEVEL)/tools/mcld/include -I$(BUILDTOP)/tools/mcld/include

mcld_debug_LDFLAGS = ${LLVM_LDFLAGS} -L$(BUILDTOP)/lib
mcld_debug_LDADD = ../lib/libmcld.debug.la
endif

dist_llvm_mcld_debug_SOURCES = ${TOOLDIR}/llvm-mcld/llvm-mcld.cpp \
    ${UNITTEST_SOURCE}

dist_mcld_debug_SOURCES = ${TOOLDIR}/mcld/main.cpp \
    ${TOOLDIR}/mcld/include/alone/Linker.h \
    ${TOOLDIR}/mcld/lib/Core/Linker.cpp \
    ${TOOLDIR}/mcld/include/alone/Support/LinkerConfig.h \
    ${TOOLDIR}/mcld/lib/Support/LinkerConfig.cpp \
    ${TOOLDIR}/mcld/include/alone/Support/TargetLinkerConfigs.h \
    ${TOOLDIR}/mcld/lib/Support/TargetLinkerConfigs.cpp \
    ${TOOLDIR}/mcld/include/alone/Support/Initialization.h \
    ${TOOLDIR}/mcld/lib/Support/Initialization.cpp \
    ${TOOLDIR}/mcld/include/alone/Support/Log.h \
    ${TOOLDIR}/mcld/include/alone/Support/MemoryFactory.h

