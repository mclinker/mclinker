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
#include "mcld/Target/GNULDBackend.h"
#include "MipsELFDynamic.h"
#include "MipsGOT.h"

namespace mcld {

class MCLinker;
class OutputRelocSection;
class SectionMap;

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
  MipsGNULDBackend();
  ~MipsGNULDBackend();

public:
  /// initTargetSectionMap - initialize target dependent section mapping.
  bool initTargetSectionMap(SectionMap& pSectionMap);

  /// initTargetSections - initialize target dependent sections in output
  void initTargetSections(MCLinker& pLinker);

  /// initTargetSymbols - initialize target dependent symbols in output.
  void initTargetSymbols(MCLinker& pLinker, const Output& pOutput);

  /// initRelocFactory - create and initialize RelocationFactory.
  bool initRelocFactory(const MCLinker& pLinker);

  /// getRelocFactory - return relocation factory.
  RelocationFactory* getRelocFactory();

  /// scanRelocation - determine the empty entries are needed or not and
  /// create the empty entries if needed.
  /// For Mips, the GOT, GP, and dynamic relocation entries are check to create.
  void scanRelocation(Relocation& pReloc,
                      const LDSymbol& pInputSym,
                      MCLinker& pLinker,
                      const MCLDInfo& pLDInfo,
                      const Output& pOutput,
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
  uint64_t abiPageSize(const MCLDInfo& pInfo) const;

  /// preLayout - Backend can do any needed modification before layout
  void doPreLayout(const Output& pOutput,
                   const MCLDInfo& pInfo,
                   MCLinker& pLinker);

  /// postLayout -Backend can do any needed modification after layout
  void doPostLayout(const Output& pOutput,
                    const MCLDInfo& pInfo,
                    MCLinker& pLinker);

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
  /// @param pOutput - the output file
  /// @param pSection - the given LDSection
  /// @param pInfo - all options in the command line.
  /// @param pLayout - for comouting the size of fragment
  /// @param pRegion - the region to write out data
  /// @return the size of the table in the file.
  uint64_t emitSectionData(const Output& pOutput,
                           const LDSection& pSection,
                           const MCLDInfo& pInfo,
                           const Layout& pLayout,
                           MemoryRegion& pRegion) const;

  /// emitNamePools - emit dynamic name pools - .dyntab, .dynstr, .hash
  virtual void emitDynNamePools(Output& pOutput,
                                SymbolCategory& pSymbols,
                                const Layout& pLayout,
                                const MCLDInfo& pLDInfo);

  MipsGOT& getGOT();
  const MipsGOT& getGOT() const;

  OutputRelocSection& getRelDyn();
  const OutputRelocSection& getRelDyn() const;

  /// getTargetSectionOrder - compute the layout order of ARM target sections
  unsigned int getTargetSectionOrder(const Output& pOutput,
                                     const LDSection& pSectHdr,
                                     const MCLDInfo& pInfo) const;

  /// finalizeSymbol - finalize the symbol value
  bool finalizeTargetSymbols(MCLinker& pLinker, const Output& pOutput);

  /// allocateCommonSymbols - allocate common symbols in the corresponding
  /// sections.
  bool allocateCommonSymbols(const MCLDInfo& pLDInfo, MCLinker& pLinker) const;

private:
  void scanLocalReloc(Relocation& pReloc,
                      const LDSymbol& pInputSym,
                      MCLinker& pLinker,
                      const MCLDInfo& pLDInfo,
                      const Output& pOutput);

  void scanGlobalReloc(Relocation& pReloc,
                      const LDSymbol& pInputSym,
                      MCLinker& pLinker,
                      const MCLDInfo& pLDInfo,
                      const Output& pOutput);

  void createGOT(MCLinker& pLinker, const Output& pOutput);
  void createRelDyn(MCLinker& pLinker, const Output& pOutput);

  /// updateAddend - update addend value of the relocation if the
  /// the target symbol is a section symbol. Addend is the offset
  /// in the section. This value should be updated after section
  /// merged.
  void updateAddend(Relocation& pReloc,
                    const LDSymbol& pInputSym,
                    const Layout& pLayout) const;

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
  /// emitDynamicSymbol - emit dynamic symbol.
  void emitDynamicSymbol(llvm::ELF::Elf32_Sym& sym32,
                         Output& pOutput,
                         LDSymbol& pSymbol,
                         const Layout& pLayout,
                         char* strtab,
                         size_t strtabsize,
                         size_t symtabIdx);
};

} // namespace of mcld

#endif

