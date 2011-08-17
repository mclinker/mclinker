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

  bool has_addend;
  uint32_t offset;  // r_offset
  int32_t addend; // r_addend

  // ELF and MachO have different interpretation way.
  // ELF: symbol_table_index(24) + r_type(8)
  // MachO: symbol_table_index(24) + r_pcrel(1)
  //                               + r_length(2)
  //                               + r_extern(1)
  //                               + r_type(4)
  uint32_t info;

  llvm::MCSymbolData* symbol;
};

struct RelocationSection {
  void push_back(const RelocationEntry& RE) { entries.push_back(RE); }
  std::vector<RelocationEntry> entries;
};


} // namespace of mcld

#endif
