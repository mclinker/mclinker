//===- HexagonGNUInfo.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "HexagonGNUInfo.h"

using namespace mcld;

/// flags - the value of ElfXX_Ehdr::e_flags
uint64_t HexagonGNUInfo::flags() const
{
  return 0x3;
}

