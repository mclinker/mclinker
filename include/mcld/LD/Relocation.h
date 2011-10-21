//===- Relocation.h -------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LD_RELOCATION_H
#define LD_RELOCATION_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <stdint.h>
#include <vector>

namespace llvm
{
  class MCSymbolData;
}

namespace mcld
{

struct RelocationEntry {
  RelocationEntry(uint32_t offset, int32_t addend, uint32_t info,
                  llvm::MCSymbolData* symbol)
    : offset(offset), addend(addend), info(info), symbol(symbol)
  {}

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

struct RelocationInfo {
  std::vector<RelocationEntry> entries;
};


} // namespace of mcld

#endif

