//===- X86ELFDynamic.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/LD/ELFFileFormat.h>
#include "X86ELFDynamic.h"

using namespace mcld;

X86ELFDynamic::X86ELFDynamic(const GNULDBackend& pParent)
  : ELFDynamic(pParent)
{
}

X86ELFDynamic::~X86ELFDynamic()
{
}

/// reservePLTGOT - reserve a DT_PLTGOT entry
void X86ELFDynamic::reservePLTGOT(const ELFFileFormat& pFormat)
{
  if (pFormat.hasGOTPLT())
    reserveOne(llvm::ELF::DT_PLTGOT);
}

/// applyPLTGOT - apply value for DT_PLTGOT entry
void X86ELFDynamic::applyPLTGOT(const ELFFileFormat& pFormat)
{
  if (pFormat.hasGOTPLT())
    applyOne(llvm::ELF::DT_PLTGOT, pFormat.getGOTPLT().addr());
}

