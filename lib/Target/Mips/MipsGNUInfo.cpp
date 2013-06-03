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
  if (m_Triple.isArch32Bit())
    return 0x400000;
  else
    return 0x120000000ull;
}

uint64_t MipsGNUInfo::flags() const
{
  uint64_t val = llvm::ELF::EF_MIPS_NOREORDER | m_PICFlags;

  if (m_Triple.isArch32Bit())
    val |= llvm::ELF::EF_MIPS_ARCH_32R2 | llvm::ELF::EF_MIPS_ABI_O32;
  else
    val |= llvm::ELF::EF_MIPS_ARCH_64R2;

  return val;
}

const char* MipsGNUInfo::entry() const
{
  return "__start";
}

const char* MipsGNUInfo::dyld() const
{
  return m_Triple.isArch32Bit() ? "/lib/ld.so.1" : "/lib64/ld.so.1";
}

uint64_t MipsGNUInfo::abiPageSize() const
{
  return 0x10000;
}

} // end mcld namespace
