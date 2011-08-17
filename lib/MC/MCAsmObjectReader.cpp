/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/MC/MCAsmObjectReader.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/ELFRelocation.h>
#include <llvm/MC/MCAssembler.h>
#include <llvm/MC/MCValue.h>
#include <llvm/MC/MCFixup.h>
#include <llvm/MC/MCFixupKindInfo.h>
#include <llvm/MC/MCSymbol.h>
#include <llvm/MC/MCAsmLayout.h>
#include <llvm/MC/MCAsmBackend.h>
#include <llvm/MC/MCAssembler.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/MC/MCObjectStreamer.h>
#include <llvm/Support/Casting.h>
#include <iostream>

using namespace llvm;
using namespace mcld;

//==========================
// MCAsmObjectReader
MCAsmObjectReader::MCAsmObjectReader(MCObjectStreamer &pStreamer,
                                     MCLDInfo& pLDInfo)
  : MCObjectWriter(llvm::nulls(),
                   pStreamer.getAssembler().getWriter().isLittleEndian()),
    m_LDInfo(pLDInfo) {
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

#if 0
static bool isFixupKindPCRel(const MCAssembler& Asm, unsigned Kind) {
  const MCFixupKindInfo &FKI =
      Asm.getBackend().getFixupKindInfo((MCFixupKind)Kind);
  return FKI.Flags & MCFixupKindInfo::FKF_IsPCRel;
}
#endif

void MCAsmObjectReader::RecordRelocation(const MCAssembler &Asm,
                                         const MCAsmLayout &Layout,
                                         const MCFragment *Fragment,
                                         const MCFixup &Fixup, MCValue Target,
                                         uint64_t &FixedValue)
{
	std::cerr << "Chris: I'm stupid" << std::endl;

#if 0
  int64_t Addend = 0;
  int Index = 0;
  int64_t Value = Target.getConstant();
  const MCSymbol* RelocSymbol = NULL;

  bool IsPCRel = isFixupKindPCRel(Asm, Fixup.getKind());
  if (!Target.isAbsolute()) {
    const MCSymbol &Symbol = Target.getSymA()->getSymbol();
    const MCSymbol &ASymbol = Symbol.AliasedSymbol();
    RelocSymbol = SymbolToReloc(Asm, Target, *Fragment, Fixup, IsPCRel);

    if (const MCSymbolRefExpr* RefB = Target.getSymB()) {
      const MCSymbol &SymbolB = RefB->getSymbol();
      MCSymbolData &SDB = Asm.getSymbolData(SymbolB);
      IsPCRel = true;

      // Offset of the symbol in the section
      int64_t a = Layout.getSymbolOffset(&SDB);

      // Ofeset of the relocation in the section
      int64_t b = Layout.getFragmentOffset(Fragment) + Fixup.getOffset();
      Value += b - a;
    }

    if (!RelocSymbol) {
      MCSymbolData &SD = Asm.getSymbolData(ASymbol);
      MCFragment *F = SD.getFragment();

      Index = F->getParent()->getOrdinal() + 1;

      // Offset of the symbol in the section
      Value += Layout.getSymbolOffset(&SD);
    } else {
      if (Asm.getSymbolData(Symbol).getFlags() & ELF_Other_Weakref)
        WeakrefUsedInReloc.insert(RelocSymbol);
      else
        UsedInReloc.insert(RelocSymbol);
      Index = -1;
    }
    Addend = Value;
    // Compensate for the addend on i386.
    if (is64Bit()) {
      Value = 0;
    }
  }

  FixedValue = Value;
  unsigned Type = GetRelocType(Target, Fixup, IsPCRel,
                               (RelocSymbol != 0), Addend);
  uint64_t RelocOffset = Layout.getFragmentOffset(Fragment)
                         + Fixup.getOffset();
  if (!hasRelocationAddend()) {
    Addend = 0;
  }
#endif

  // Record relocation information into MCLDContext of the bitcode
  ELFRelocationInfo& ERI = m_LDInfo.bitcode().context()->getRelocationInfo();
  ELFRelocationEntry ERE(Offset, Addend, Index, Type, Symbol);
  ERI.RelocEntries[Fragment->getParent()].push_back(ERE);
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

