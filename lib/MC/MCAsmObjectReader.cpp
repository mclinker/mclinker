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
#include <llvm/MC/MCObjectStreamer.h>
#include <llvm/Support/Casting.h>
#include <iostream>

using namespace llvm;
using namespace mcld;

//==========================
// MCAsmObjectReader
MCAsmObjectReader::MCAsmObjectReader(MCObjectStreamer &pStreamer,
                                     MCLDFile &pDefaultBitcode)
  : MCObjectWriter(llvm::nulls(), 
                   pStreamer.getAssembler().getWriter().isLittleEndian()),
    m_DftBitcode(pDefaultBitcode) {
  pStreamer.getAssembler().setWriter(*this);
}

MCAsmObjectReader::~MCAsmObjectReader()
{
}

void MCAsmObjectReader::ExecutePostLayoutBinding(MCAssembler &Asm,
                                                 const MCAsmLayout &Layout)
{
	std::cerr << "Csmon: I'm the king of the world" << std::endl;
}

void MCAsmObjectReader::RecordRelocation(const MCAssembler &Asm,
                                         const MCAsmLayout &Layout,
                                         const MCFragment *Fragment,
                                         const MCFixup &Fixup, MCValue Target,
                                         uint64_t &FixedValue)
{
	std::cerr << "Chris: I'm stupid" << std::endl;
}

bool
MCAsmObjectReader::IsSymbolRefDifferenceFullyResolvedImpl(
                                         const MCAssembler &Asm,
                                         const MCSymbolData &DataA,
                                         const MCFragment &FB,
                                         bool InSet,
                                         bool IsPCRel) const
{
	std::cerr << "Duo: I love nowar" << std::endl;
}

void MCAsmObjectReader::WriteObject(MCAssembler &Asm,
                                    const MCAsmLayout &Layout)
{
	std::cerr << "Nowar: I'm cleaver" << std::endl;
}

