//===- MipsGNUInfo.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "MipsGNUInfo.h"

namespace mcld {

//===----------------------------------------------------------------------===//
// MipsGNUInfo
//===----------------------------------------------------------------------===//
MipsGNUInfo::MipsGNUInfo(const llvm::Triple& pTriple)
  : GNUInfo(pTriple),
    m_ABIVersion(0),
    m_PICFlags(0)
{}

void MipsGNUInfo::setABIVersion(uint8_t ver)
{
  m_ABIVersion = ver;
}

void MipsGNUInfo::setPICFlags(uint64_t flags)
{
  m_PICFlags = flags;
}

uint32_t MipsGNUInfo::machine() const
{
  return llvm::ELF::EM_MIPS;
}

uint8_t MipsGNUInfo::ABIVersion() const
{
  return m_ABIVersion;
}

uint64_t MipsGNUInfo::defaultTextSegmentAddr() const
{
  return 0x400000;
}

uint64_t MipsGNUInfo::flags() const
{
  return llvm::ELF::EF_MIPS_ARCH_32R2 |
         llvm::ELF::EF_MIPS_NOREORDER |
         llvm::ELF::EF_MIPS_ABI_O32 |
         m_PICFlags;
}

const char* MipsGNUInfo::entry() const
{
  return "__start";
}

const char* MipsGNUInfo::dyld() const
{
  return "/lib/ld.so.1";
}

uint64_t MipsGNUInfo::abiPageSize() const
{
  return 0x10000;
}

} // end mcld namespace
