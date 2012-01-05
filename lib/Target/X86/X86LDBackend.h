//===- X86LDBackend.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef X86_LDBACKEND_H
#define X86_LDBACKEND_H
#include "mcld/Target/GNULDBackend.h"
#include "X86GOT.h"

namespace mcld {

//===----------------------------------------------------------------------===//
/// X86GNULDBackend - linker backend of X86 target of GNU ELF format
///
class X86GNULDBackend : public GNULDBackend
{
public:
  X86GNULDBackend();

  ~X86GNULDBackend();

  RelocationFactory* getRelocFactory();

  uint32_t machine() const;

  bool isLittleEndian() const
  { return true; }

  X86GOT& getGOT();

  const X86GOT& getGOT() const;

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
  /// OSABI - the value of e_ident[EI_OSABI]
  /// FIXME
  uint8_t OSABI() const
  { return llvm::ELF::ELFOSABI_NONE; }
  
  /// ABIVersion - the value of e_ident[EI_ABIVRESION]
  /// FIXME
  uint8_t ABIVersion() const
  { return 0x0; }

  /// flags - the value of ElfXX_Ehdr::e_flags
  /// FIXME
  uint64_t flags() const
  { return 0x0; } 

  /// initTargetSectionMap - initialize target dependent section mapping
  bool initTargetSectionMap(SectionMap& pSectionMap);

  // initRelocFactory - create and initialize RelocationFactory
  bool initRelocFactory(const MCLinker& pLinker);

  void initTargetSections(MCLinker& pLinker);

  /// scanRelocation - determine the empty entries are needed or not and create
  /// the empty entries if needed.
  /// For X86, following entries are check to create:
  /// - GOT entry (for .got and .got.plt sections)
  /// - PLT entry (for .plt section)
  /// - dynamin relocation entries (for .rel.plt and .rel.dyn sections)
  void scanRelocation(Relocation& pReloc,
                      MCLinker& pLinker,
                      unsigned int pType);

private:
  RelocationFactory* m_pRelocFactory;
  X86GOT* m_pGOT;
};

//===----------------------------------------------------------------------===//
/// X86MachOLDBackend - linker backend of X86 target of MachO format
///
/**
class X86MachOLDBackend : public DarwinX86LDBackend
{
public:
  X86MachOLDBackend();
  ~X86MachOLDBackend();

private:
  MCMachOTargetArchiveReader *createTargetArchiveReader() const;
  MCMachOTargetObjectReader *createTargetObjectReader() const;
  MCMachOTargetObjectWriter *createTargetObjectWriter() const;

};
**/
} // namespace of mcld

#endif

