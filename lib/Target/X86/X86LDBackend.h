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

  /// initTargetSectionMap - initialize target dependent section mapping
  bool initTargetSectionMap(SectionMap& pSectionMap);

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

