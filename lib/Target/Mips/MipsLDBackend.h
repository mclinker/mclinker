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
  MipsGNULDBackend();
  ~MipsGNULDBackend();

  RelocationFactory* getRelocFactory();

  uint32_t machine() const;

  /// OSABI - the value of e_ident[EI_OSABI]
  /// FIXME:
  uint8_t OSABI() const
  { return llvm::ELF::ELFOSABI_NONE; }

  /// ABIVersion - the value of e_ident[EI_ABIVRESION]
  /// FIXME:
  uint8_t ABIVersion() const
  { return 0x0; }

  /// flags - the value of ElfXX_Ehdr::e_flags
  uint64_t flags() const;

  bool isLittleEndian() const;

  unsigned int bitclass() const;

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
  /// Backend can put the data of the tables in MCSectionData directly
  ///  - LDSection.getSectionData can get the section data.
  /// Or, backend can put the data into special data structure
  ///  - backend can maintain its own map<LDSection, table> to get the table
  /// from given LDSection.
  ///
  /// @param pOutput - the output file
  /// @param pSection - the given LDSection
  /// @param pInfo - all options in the command line.
  /// @param pRegion - the region to write out data
  /// @return the size of the table in the file.
  uint64_t emitSectionData(const Output& pOutput,
                           const LDSection& pSection,
                           const MCLDInfo& pInfo,
                           MemoryRegion& pRegion) const {
    // FIXME
    return 0x0;
  }

  MipsGOT& getGOT();

  const MipsGOT& getGOT() const;

  bool initTargetSectionMap(SectionMap& pSectionMap);

  void initTargetSections(MCLinker& pLinker);

  void initTargetSymbols(MCLinker& pLinker);

  // initRelocFactory - create and initialize RelocationFactory
  bool initRelocFactory(const MCLinker& pLinker);

  /// scanRelocation - determine the empty entries are needed or not and
  /// create the empty entries if needed.
  /// For Mips, the GOT, GP, and dynamic relocation entries are check to create.
  void scanRelocation(Relocation& pReloc,
                      MCLinker& pLinker,
                      const MCLDInfo& pLDInfo,
                      const Output& pOutput);

  /// finalizeSymbol - finalize the symbol value
  /// If the symbol's reserved field is not zero, MCLinker will call back this
  /// function to ask the final value of the symbol
  bool finalizeSymbol(LDSymbol& pSymbol) const;

private:
  RelocationFactory* m_pRelocFactory;

  std::auto_ptr<MipsGOT> m_pGOT;                     // .got
  std::auto_ptr<OutputRelocSection> m_pRelDynSec;    // .rel.dyn

  MipsELFDynamic* m_pDynamic;
private:
  void scanLocalRelocation(Relocation& pReloc,
                           MCLinker& pLinker,
                           const Output& pOutput);

  void scanGlobalRelocation(Relocation& pReloc,
                            MCLinker& pLinker,
                            const Output& pOutput);

  void createGOTSec(MCLinker& pLinker);
  void createRelDynSec(MCLinker& pLinker);
  void createPltSec(MCLinker& pLinker);
  void createMipsStubsSec(MCLinker& pLinker);

  bool isSymbolNeedsDynRel(ResolveInfo& pSym, const Output& pOutput) const;
  bool isSymbolNeedsPLT(ResolveInfo& pSym, const Output& pOutput) const;
};

} // namespace of mcld

#endif

