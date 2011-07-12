/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/MC/MCAsmObjectReader.h>
#include <llvm/MC/MCAssembler.h>
#include <llvm/MC/MCValue.h>
#include <llvm/MC/MCFixup.h>
#include <llvm/MC/MCSymbol.h>
#include <llvm/MC/MCAsmLayout.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;
using namespace mcld;

//==========================
// MCAsmObjectReader
MCAsmObjectReader::MCAsmObjectReader(const llvm::MCObjectWriter& pWriter)
  : MCObjectWriter(llvm::nulls(), pWriter.isLittleEndian()) {
}

MCAsmObjectReader::~MCAsmObjectReader()
{
}

void MCAsmObjectReader::ExecutePostLayoutBinding(MCAssembler &Asm,
                                                 const MCAsmLayout &Layout)
{
}

void MCAsmObjectReader::RecordRelocation(const MCAssembler &Asm,
                                         const MCAsmLayout &Layout,
                                         const MCFragment *Fragment,
                                         const MCFixup &Fixup, MCValue Target,
                                         uint64_t &FixedValue)
{
}

bool
MCAsmObjectReader::IsSymbolRefDifferenceFullyResolvedImpl(
                                         const MCAssembler &Asm,
                                         const MCSymbolData &DataA,
                                         const MCFragment &FB,
                                         bool InSet,
                                         bool IsPCRel) const
{
}

void MCAsmObjectReader::WriteObject(MCAssembler &Asm,
                                    const MCAsmLayout &Layout)
{
}

