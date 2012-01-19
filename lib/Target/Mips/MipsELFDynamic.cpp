//===- MipsELFDynamic.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/Support/ErrorHandling.h>

#include "MipsELFDynamic.h"

using namespace mcld;

MipsELFDynamic::MipsELFDynamic(const GNULDBackend& pParent)
  : ELFDynamic(pParent)
{
}

MipsELFDynamic::~MipsELFDynamic()
{
}

/// reservePLTGOT - reserve a DT_PLTGOT entry
void MipsELFDynamic::reservePLTGOT(const ELFFileFormat& pFormat)
{
  // FIXME: unsupport reserving DT_PLTGOT entry for Mips
  llvm::report_fatal_error("unsupport reserving DT_PLTGOT entry for Mips!");
}

/// applyPLTGOT - apply value for DT_PLTGOT entry
void MipsELFDynamic::applyPLTGOT(const ELFFileFormat& pFormat)
{
  // FIXME: unsupport applying value to DT_PLTGOT entry for Mips
  llvm::report_fatal_error(
    "unsupport applying value to DT_PLTGOT entry for Mips!");
}

