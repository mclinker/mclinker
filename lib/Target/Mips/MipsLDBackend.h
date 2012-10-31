//===- MipsLDBackend.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MIPS_LDBACKEND_H
#define MIPS_LDBACKEND_H
#include <mcld/Target/GNULDBackend.h>
#include "MipsELFDynamic.h"
#include "MipsGOT.h"

namespace mcld {

class LinkerConfig;
class FragmentLinker;
class OutputRelocSection;
class SectionMap;
class MemoryArea;

//===----------------------------------------------------------------------===//
/// MipsGNULDBackend - linker backend of Mips target of GNU ELF format
///
class MipsGNULDBackend : public GNULDBackend
{
public:
  enum ReservedEntryType {
    None          = 0,  // no reserved entry
    ReserveRel    = 1,  // reserve a dynamic relocation entry
    ReserveGot    = 2,  // reserve a GOT entry
    ReserveGpDisp = 8   // reserve _gp_disp symbol
  };

public:
  MipsGNULDBackend(const LinkerConfig& pConfig);
  ~MipsGNULDBackend();

public:
  /// initTargetSections - initialize target dependent sections in output
  void initTargetSections(Module& pModule, ObjectBuilder& pBuilder);

  /// initTargetSymbols - initialize target dependent symbols in output.
  void initTargetSymbols(FragmentLinker& pLinker);

  /// initRelocFactory - create and initialize RelocationFactory.
  bool initRelocFactory(const FragmentLinker& pLinker);

  /// getRelocFactory - return relocation factory.
  RelocationFactory* getRelocFactory();

  /// scanRelocation - determine the empty entries are needed or not and
  /// create the empty entries if needed.
  /// For Mips, the GOT, GP, and dynamic relocation entries are check to create.
  void scanRelocation(Relocation& pReloc,
                      FragmentLinker& pLinker,
                      Module& pModule,
                      const LDSection& pSection);

  uint32_t machine() const;

  /// OSABI - the value of e_ident[EI_OSABI]
  uint8_t OSABI() const;

  /// ABIVersion - the value of e_ident[EI_ABIVRESION]
  uint8_t ABIVersion() const;

  /// flags - the value of ElfXX_Ehdr::e_flags
  uint64_t flags() const;

  bool isLittleEndian() const;

  unsigned int bitclass() const;

  uint64_t defaultTextSegmentAddr() const;

  /// abiPageSize - the abi page size of the target machine
  uint64_t abiPageSize() const;

  /// preLayout - Backend can do any needed modification before layout
  void doPreLayout(FragmentLinker& pLinker);

  /// postLayout -Backend can do any needed modification after layout
  void doPostLayout(Module& pModule, FragmentLinker& pLinker);

  /// dynamic - the dynamic section of the target machine.
  /// Use co-variant return type to return its own dynamic section.
  MipsELFDynamic& dynamic();

  /// dynamic - the dynamic section of the target machine.
  /// Use co-variant return type to return its own dynamic section.
  const MipsELFDynamic& dynamic() const;

  /// emitSectionData - write out the section data into the memory region.
  /// When writers get a LDSection whose kind is LDFileFormat::Target, writers
  /// call back target backend to emit the data.
  ///
  /// Backends handle the target-special tables (plt, gp,...) by themselves.
  /// Backend can put the data of the tables in SectionData directly
  ///  - LDSection.getSectionData can get the section data.
  /// Or, backend can put the data into special data structure
  ///  - backend can maintain its own map<LDSection, table> to get the table
  /// from given LDSection.
  ///
  /// @param pSection - the given LDSection
  /// @param pRegion - the region to write out data
  /// @return the size of the table in the file.
  uint64_t emitSectionData(const LDSection& pSection,
                           MemoryRegion& pRegion) const;

  void sizeNamePools(const Module& pModule, bool pIsStaticLink);

  /// emitNamePools - emit dynamic name pools - .dyntab, .dynstr, .hash
  void emitDynNamePools(const Module& pModule, MemoryArea& pOut);


  MipsGOT& getGOT();
  const MipsGOT& getGOT() const;

  OutputRelocSection& getRelDyn();
  const OutputRelocSection& getRelDyn() const;

  /// getTargetSectionOrder - compute the layout order of ARM target sections
  unsigned int getTargetSectionOrder(const LDSection& pSectHdr) const;

  /// finalizeSymbol - finalize the symbol value
  bool finalizeTargetSymbols(FragmentLinker& pLinker);

  /// allocateCommonSymbols - allocate common symbols in the corresponding
  /// sections.
  bool allocateCommonSymbols(Module& pModule);

private:
  void scanLocalReloc(Relocation& pReloc, FragmentLinker& pLinker);

  void scanGlobalReloc(Relocation& pReloc, FragmentLinker& pLinker);

  void defineGOTSymbol(FragmentLinker& pLinker);

  /// emitSymbol32 - emit an ELF32 symbol, override parent's function
  void emitSymbol32(llvm::ELF::Elf32_Sym& pSym32,
                    LDSymbol& pSymbol,
                    char* pStrtab,
                    size_t pStrtabsize,
                    size_t pSymtabIdx);

  /// getRelEntrySize - the size in BYTE of rel type relocation
  size_t getRelEntrySize()
  { return 8; }

  /// getRelEntrySize - the size in BYTE of rela type relocation
  size_t getRelaEntrySize()
  { return 12; }

  /// doCreateProgramHdrs - backend can implement this function to create the
  /// target-dependent segments
  virtual void doCreateProgramHdrs(Module& pModule,
                                   const FragmentLinker& pLinker);

private:
  RelocationFactory* m_pRelocFactory;

  MipsGOT* m_pGOT;                      // .got
  OutputRelocSection* m_pRelDyn;        // .rel.dyn

  MipsELFDynamic* m_pDynamic;
  LDSymbol* m_pGOTSymbol;
  LDSymbol* m_pGpDispSymbol;

  std::vector<LDSymbol*> m_GlobalGOTSyms;

private:
  /// isGlobalGOTSymbol - return true if the symbol is the global GOT entry.
  bool isGlobalGOTSymbol(const LDSymbol& pSymbol) const;

};

} // namespace of mcld

#endif

