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
#include "MipsGOT.h"

namespace mcld {

class MCLinker;
class MipsDynRelSection;
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
  /// FIXME:
  uint64_t flags() const
  { return llvm::ELF::EF_MIPS_ARCH_32; }

  bool isLittleEndian() const;

  unsigned int bitclass() const;

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
  /// @param pSection - the given LDSection
  /// @param pRegion - the region to write out data
  /// @return the size of the table in the file.
  uint64_t emitSectionData(const LDSection& pSection,
                           MemoryRegion& pRegion) const {
    // FIXME
    return 0x0;
  }

  MipsGOT& getGOT();

  const MipsGOT& getGOT() const;

  bool initTargetSectionMap(SectionMap& pSectionMap);

  void initTargetSections(MCLinker& pLinker);

  /// scanRelocation - determine the empty entries are needed or not and
  /// create the empty entries if needed.
  /// For Mips, the GOT, GP, and dynamic relocation entries are check to create.
  /// (or there are any other target dependent entries for Mips?)
  void scanRelocation(Relocation& pReloc,
                      MCLinker& pLinker,
                      unsigned int pType);

private:
  RelocationFactory* m_pRelocFactory;

  std::auto_ptr<MipsGOT> m_pGOT;                    // .got
  std::auto_ptr<MipsDynRelSection> m_pRelDynSec;    // .rel.dyn

private:
  void scanLocalRelocation(Relocation& pReloc,
                           MCLinker& pLinker,
                           unsigned int pType);

  void scanGlobalRelocation(Relocation& pReloc,
                            MCLinker& pLinker,
                            unsigned int pType);

  void createGOTSec(MCLinker& pLinker);
  void createRelDynSec(MCLinker& pLinker);
  void createPltSec(MCLinker& pLinker);
  void createMipsStubsSec(MCLinker& pLinker);

  bool isSymbolNeedsDynRel(ResolveInfo& pSym, unsigned int pType);
  bool isSymbolNeedsPLT(ResolveInfo& pSym, unsigned int pType);
};

} // namespace of mcld

#endif

