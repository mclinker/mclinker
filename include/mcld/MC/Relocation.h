/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef RELOCATION_H_
#define RELOCATION_H_
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <vector>

namespace mcld
{

struct RelocationEntry {
  RelocationEntry()
    : offset(0), addend(0), info(0), symbol(NULL)
  {}
  RelocationEntry(uint32_t offset, int32_t addend, uint32_t info,
                  llvm::MCSymbolData* symbol)
    : offset(offset), addend(addend), info(info), symbol(symbol)
  {}

  uint32_t offset;  // r_offset
  int32_t addend; // r_addend
  uint32_t info;  // ELF and MachO have different ways to interpret
  llvm::MCSymbolData* symbol;
};

class RelocationSection {
public:
  void push_back(const RelocationEntry& RE) { entries.push_back(RE); }
private:
  std::vector<RelocationEntry> entries;
};


} // namespace of mcld

#endif
