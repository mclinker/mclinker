//===- MipsELFDynamic.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/LD/ELFFileFormat.h>
#include <mcld/Target/GNULDBackend.h>
#include "MipsELFDynamic.h"

using namespace mcld;

// MIPS mandatory dynamic section entries
enum {
  MIPS_RLD_VERSION  = 0x70000001,
  MIPS_FLAGS        = 0x70000005,
  MIPS_BASE_ADDRESS = 0x70000006,
  MIPS_LOCAL_GOTNO  = 0x7000000a,
  MIPS_SYMTABNO     = 0x70000011,
  MIPS_GOTSYM       = 0x70000013,
};

MipsELFDynamic::MipsELFDynamic(const GNULDBackend& pParent)
  : ELFDynamic(pParent)
{
}

MipsELFDynamic::~MipsELFDynamic()
{
}

void MipsELFDynamic::reserveTargetEntries(const ELFFileFormat& pFormat)
{
  // reservePLTGOT
  if (pFormat.hasGOT())
    reserveOne(llvm::ELF::DT_PLTGOT);

  reserveOne(MIPS_RLD_VERSION);
  reserveOne(MIPS_FLAGS);
  reserveOne(MIPS_BASE_ADDRESS);
  reserveOne(MIPS_LOCAL_GOTNO);
  reserveOne(MIPS_SYMTABNO);
  reserveOne(MIPS_GOTSYM);
}

void MipsELFDynamic::applyTargetEntries(const ELFFileFormat& pFormat)
{
  // applyPLTGOT
  if (pFormat.hasGOT())
    applyOne(llvm::ELF::DT_PLTGOT, pFormat.getGOT().addr());

  applyOne(MIPS_RLD_VERSION, 1);
  applyOne(MIPS_FLAGS, 0);
  applyOne(MIPS_BASE_ADDRESS, 0);
  applyOne(MIPS_LOCAL_GOTNO, 0);
  applyOne(MIPS_SYMTABNO, getSymTabNo(pFormat));
  applyOne(MIPS_GOTSYM, 0);
}

uint64_t MipsELFDynamic::getSymTabNo(const ELFFileFormat& pFormat) const
{
  if (!pFormat.hasDynSymTab())
    return 0;

  const LDSection& dynsym = pFormat.getDynSymTab();
  return dynsym.size() / symbolSize();
}
