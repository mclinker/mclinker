/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/MC/MCAsmObjectReader.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/Relocation.h>
#include <llvm/MC/MCAssembler.h>
#include <llvm/MC/MCExpr.h>
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

static bool isFixupKindPCRel(const MCAssembler &Asm, unsigned Kind) {
  const MCFixupKindInfo &FKI =
    Asm.getBackend().getFixupKindInfo((MCFixupKind) Kind);

  return FKI.Flags & MCFixupKindInfo::FKF_IsPCRel;
}

#if 0
const MCSymbol *ELFObjectWriter::SymbolToReloc(const MCAssembler &Asm,
                                               const MCValue &Target,
                                               const MCFragment &F,
                                               const MCFixup &Fixup,
                                               bool IsPCRel) const {
  const MCSymbol &Symbol = Target.getSymA()->getSymbol();
  const MCSymbol &ASymbol = Symbol.AliasedSymbol();
  const MCSymbol *Renamed = Renames.lookup(&Symbol);
  const MCSymbolData &SD = Asm.getSymbolData(Symbol);

  if (ASymbol.isUndefined()) {
    if (Renamed)
      return Renamed;
    return &ASymbol;
  }

  if (SD.isExternal()) {
    if (Renamed)
      return Renamed;
    return &Symbol;
  }

  const MCSectionELF &Section =
    static_cast<const MCSectionELF&>(ASymbol.getSection());
  const SectionKind secKind = Section.getKind();

  if (secKind.isBSS())
    return ExplicitRelSym(Asm, Target, F, Fixup, IsPCRel);

  if (secKind.isThreadLocal()) {
    if (Renamed)
      return Renamed;
    return &Symbol;
  }

  MCSymbolRefExpr::VariantKind Kind = Target.getSymA()->getKind();
  const MCSectionELF &Sec2 =
    static_cast<const MCSectionELF&>(F.getParent()->getSection());

  if (&Sec2 != &Section &&
      (Kind == MCSymbolRefExpr::VK_PLT ||
       Kind == MCSymbolRefExpr::VK_GOTPCREL ||
       Kind == MCSymbolRefExpr::VK_GOTOFF)) {
    if (Renamed)
      return Renamed;
    return &Symbol;
  }

  if (Section.getFlags() & ELF::SHF_MERGE) {
    if (Target.getConstant() == 0)
      return ExplicitRelSym(Asm, Target, F, Fixup, IsPCRel);
    if (Renamed)
      return Renamed;
    return &Symbol;
  }

  return ExplicitRelSym(Asm, Target, F, Fixup, IsPCRel);
}
#endif

void MCAsmObjectReader::RecordRelocation(const MCAssembler &Asm,
                                         const MCAsmLayout &Layout,
                                         const MCFragment *Fragment,
                                         const MCFixup &Fixup, MCValue Target,
                                         uint64_t &FixedValue)
{
	std::cerr << "Nowar: I love bugs!" << std::endl;

  int64_t Addend = 0;
  int Index = 0;
  int64_t Value = Target.getConstant();
  const MCSymbol *RelocSymbol = NULL;
  unsigned Type= 0;

  const MCFixupKindInfo& FKI =
    Asm.getBackend().getFixupKindInfo((MCFixupKind)Fixup.getKind());
  bool IsPCRel = isFixupKindPCRel(Asm, Fixup.getKind());

  if (!Target.isAbsolute()) {
    const MCSymbol &Symbol = Target.getSymA()->getSymbol();
    const MCSymbol &ASymbol = Symbol.AliasedSymbol();
    // FIXME(Nowar): Need MCELFObjectTargetWriter
    //RelocSymbol = SymbolToReloc(Asm, Target, *Fragment, Fixup, IsPCRel);

    if (const MCSymbolRefExpr* RefB = Target.getSymB()) {
      const MCSymbol& SymbolB = RefB->getSymbol();
      MCSymbolData& SDB = Asm.getSymbolData(SymbolB);
      IsPCRel = true;

      // Offset of the symbol in the section
      int64_t a = Layout.getSymbolOffset(&SDB);

      // Offset of the relocation in the section
      int64_t b = Layout.getFragmentOffset(Fragment) + Fixup.getOffset();
      Value += b-a;
    }

    if (!RelocSymbol) {
      MCSymbolData& SD = Asm.getSymbolData(ASymbol);
      MCFragment* F = SD.getFragment();

      // Offset of the symbol in the section
      Value += Layout.getSymbolOffset(&SD);
    }

    Addend = Value;
  }

  FixedValue = Value;
  // FIXME(Nowar): Need MCELFObjectTargetWriter
  //Type = GetRelocType(Target, Fixup, IsPCRel,
  //                    (RelocSymbol != 0), Addend);
  MCSymbolData& SD = Asm.getSymbolData(*RelocSymbol);
  unsigned Info = (SD.Index << 8) + (unsigned char)Type;
  uint64_t RelocOffset = Layout.getFragmentOffset(Fragment) + Fixup.getOffset();

  // FIXME(Nowar): Need MCELFObjectTargetWriter
  //if (!hasRelocationAddend()) Addend = 0;

  RelocationEntry RE(RelocOffset, Addend, Info, &SD);
  m_LDInfo.bitcode().context()->getRelocSection().push_back(RE);
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

