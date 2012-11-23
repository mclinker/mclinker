//===-- llvm/Target/TargetLDBackend.h - Target LD Backend -----*- C++ -*-===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TARGET_TARGETLDBACKEND_H
#define MCLD_TARGET_TARGETLDBACKEND_H

#include <llvm/Support/DataTypes.h>

namespace mcld {

class Module;
class LinkerConfig;
class FragmentLinker;
class Relocation;
class RelocationFactory;
class Layout;
class ArchiveReader;
class ObjectReader;
class DynObjReader;
class ObjectWriter;
class DynObjWriter;
class ExecWriter;
class LDFileFormat;
class LDSymbol;
class LDSection;
class SectionData;
class Input;
class GOT;
class MemoryArea;
class MemoryAreaFactory;
class BranchIslandFactory;
class StubFactory;
class ObjectBuilder;

//===----------------------------------------------------------------------===//
/// TargetLDBackend - Generic interface to target specific assembler backends.
//===----------------------------------------------------------------------===//
class TargetLDBackend
{
  TargetLDBackend(const TargetLDBackend &);   // DO NOT IMPLEMENT
  void operator=(const TargetLDBackend &);  // DO NOT IMPLEMENT

protected:
  TargetLDBackend(const LinkerConfig& pConfig);

public:
  virtual ~TargetLDBackend();

  // -----  target dependent  ----- //
  virtual void initTargetSegments(FragmentLinker& pLinker) { }
  virtual void initTargetSections(Module& pModule, ObjectBuilder& pBuilder) { }
  virtual void initTargetSymbols(FragmentLinker& pLinker) { }
  virtual void initTargetRelocation(FragmentLinker& pLinker) { }
  virtual bool initStandardSymbols(FragmentLinker& pLinker, Module& pModule) = 0;
  virtual bool initRelocFactory(const FragmentLinker& pLinker) = 0;

  virtual RelocationFactory* getRelocFactory() = 0;

  /// scanRelocation - When read in relocations, backend can do any modification
  /// to relocation and generate empty entries, such as GOT, dynamic relocation
  /// entries and other target dependent entries. These entries are generated
  /// for layout to adjust the ouput offset.
  /// @param pReloc - a read in relocation entry
  /// @param pInputSym - the input LDSymbol of relocation target symbol
  /// @param pSection - the section of relocation applying target
  virtual void scanRelocation(Relocation& pReloc,
                              FragmentLinker& pLinker,
                              Module& pModule,
                              const LDSection& pSection) = 0;

  /// partialScanRelocation - When doing partial linking, backend can do any
  /// modification to relocation to fix the relocation offset after section
  /// merge
  /// @param pReloc - a read in relocation entry
  /// @param pInputSym - the input LDSymbol of relocation target symbol
  /// @param pSection - the section of relocation applying target
  virtual void partialScanRelocation(Relocation& pReloc,
                                     FragmentLinker& pLinker,
                                     Module& pModule,
                                     const LDSection& pSection) = 0;

  // -----  format dependent  ----- //
  virtual ArchiveReader* createArchiveReader(Module&) = 0;
  virtual ObjectReader*  createObjectReader(FragmentLinker&) = 0;
  virtual DynObjReader*  createDynObjReader(FragmentLinker&) = 0;
  virtual ObjectWriter*  createObjectWriter(FragmentLinker&) = 0;
  virtual DynObjWriter*  createDynObjWriter(FragmentLinker&) = 0;
  virtual ExecWriter*    createExecWriter(FragmentLinker&) = 0;

  virtual bool initStdSections(ObjectBuilder& pBuilder) = 0;

  /// preLayout - Backend can do any needed modification before layout
  virtual void preLayout(Module& pModule, FragmentLinker& pLinker) = 0;

  /// postLayout -Backend can do any needed modification after layout
  virtual void postLayout(Module& pModule, FragmentLinker& pLinker) = 0;

  /// postProcessing - Backend can do any needed modification in the final stage
  virtual void postProcessing(FragmentLinker& pLinker,
                              MemoryArea& pOutput) = 0;

  /// Is the target machine little endian? **/
  virtual bool isLittleEndian() const = 0;

  /// bit class. the bit length of the target machine, 32 or 64 **/
  virtual unsigned int bitclass() const = 0;

  /// the common page size of the target machine
  virtual uint64_t commonPageSize() const = 0;

  /// the abi page size of the target machine
  virtual uint64_t abiPageSize() const = 0;

  /// section start offset in the output file
  virtual size_t sectionStartOffset() const = 0;

  /// computeSectionOrder - compute the layout order of the given section
  virtual unsigned int getSectionOrder(const LDSection& pSectHdr) const = 0;

  /// sizeNamePools - compute the size of regular name pools
  /// In ELF executable files, regular name pools are .symtab, .strtab.,
  /// .dynsym, .dynstr, and .hash
  virtual void
  sizeNamePools(const Module& pModule, bool pIsStaticLink) = 0;

  /// finalizeSymbol - Linker checks pSymbol.reserved() if it's not zero,
  /// then it will ask backend to finalize the symbol value.
  /// @return ture - if backend set the symbol value sucessfully
  /// @return false - if backend do not recognize the symbol
  virtual bool finalizeSymbols(FragmentLinker& pLinker) = 0;

  /// finalizeTLSSymbol - Linker asks backend to set the symbol value when it
  /// meets a TLS symbol
  virtual bool finalizeTLSSymbol(LDSymbol& pSymbol) = 0;

  /// allocateCommonSymbols - allocate common symbols in the corresponding
  /// sections.
  virtual bool allocateCommonSymbols(Module& pModule) = 0;

  /// mergeSection - merge target dependent sections.
  virtual bool mergeSection(Module& pModule, LDSection& pInputSection)
  { return true; }

  /// readSection - read a target dependent section
  virtual bool readSection(Input& pInput, SectionData& pSD)
  { return true; }

  /// dyld - the name of the default dynamic linker
  virtual const char* dyld() const = 0;

  /// sizeInterp - compute the size of program interpreter's name
  /// In ELF executables, this is the length of dynamic linker's path name
  virtual void sizeInterp() = 0;

  // -----  relaxation  ----- //
  virtual bool initBRIslandFactory() = 0;
  virtual bool initStubFactory() = 0;
  virtual bool initTargetStubs(FragmentLinker& pLinker) { return true; }

  virtual BranchIslandFactory* getBRIslandFactory() = 0;
  virtual StubFactory*         getStubFactory() = 0;

  /// relax - the relaxation pass
  virtual bool relax(Module& pModule, FragmentLinker& pLinker) = 0;

  /// mayRelax - return true if the backend needs to do relaxation
  virtual bool mayRelax() = 0;

protected:
  const LinkerConfig& config() const { return m_Config; }

private:
  const LinkerConfig& m_Config;
};

} // End mcld namespace

#endif
