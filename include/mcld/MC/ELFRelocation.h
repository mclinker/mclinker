/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef ELF_RELOCATION_H_
#define ELF_RELOCATION_H_
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <vector>
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/SmallPtrSet.h>
#include <mcld/MC/Relocation.h>

namespace llvm {
  class MCSymbol;
  class MCSectionData;
}

namespace mcld {
  using namespace llvm;

struct ELFRelocationEntry : public RelocationEntry {
  ELFRelocationEntry(uint64_t offset, int64_t addend,
                     uint64_t index, uint64_t type,
                     const MCSymbol* sym)
    : m_Offset(offset), m_Addend(addend),
      m_Index(index), m_Type(type), m_Symbol(sym)
  {}

  // Make it large enough for ELF 32-bit and 64-bit
  uint64_t m_Offset;
  int64_t m_Addend;
  unsigned m_Index;   // symbol table index
  unsigned m_Type;  // Target-dependent relocation type
  const MCSymbol* m_Symbol; // Relocation Symbol
};

struct ELFRelocationInfo : public RelocationInfo {
  DenseMap<const MCSectionData*, std::vector<ELFRelocationEntry> > RelocEntries;

  SmallPtrSet<const MCSymbol*, 16> UsedInReloc;
  SmallPtrSet<const MCSymbol*, 16> WeakrefUsedInReloc;
  DenseMap<const MCSymbol*, const MCSymbol*> Renames;
};

#if 0
struct SectionRelocData {
  // index of section header array
  unsigned int m_Index;
  // section type
  unsigned int m_SectType;
  // sh_size / reloc_size  TODO(Nowar): Why can we get counts by this?
  unsigned int m_RelocEntryCount;
  // output section  TODO(Nowar): Determine the output sectuib data structure
  //OutputSection* m_pOutputSection;
};
#endif

} // namespace of mcld

#endif
