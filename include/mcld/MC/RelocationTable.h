/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar <nowar100@gmail.com>                                              *
 ****************************************************************************/
#ifndef RELOCDATA_H
#define RELOCDATA_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <vector>

namespace llvm {
  class MCFixup;
}

namespace mcld
{
  struct RelocationEntry {
  };

  struct RelocationTable {
    RelocationTable() {}

    // FIXME(Nowar): Better data structure?
    std::vector<RelocationEntry> RelocEntry;
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

/** \class RelocData
 *  \brief Information of relocations in an object file.
 *
 *  \see
 *  \author Nowar <nowar100@gmail.com>
 */
struct RelocData
{
  //std::vector<SectionRelocData> m_Relocs;
  std::vector<SectionRelocData *> m_Data;
};

#endif

} // namespace of mcld

#endif

