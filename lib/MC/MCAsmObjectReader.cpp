//===- MCAsmObjectReader.cpp ----------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/Relocation.h"
#include "mcld/MC/MCAsmObjectReader.h"
#include "mcld/MC/MCLDInfo.h"
#include "mcld/MC/MCObjectReader.h"
#include "mcld/Target/TargetLDBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCValue.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCAsmLayout.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/MC/MCObjectStreamer.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ELF.h"

using namespace mcld;

//===----------------------------------------------------------------------===//
// non-member functions
static bool isFixupKindPCRel(const MCAssembler &Asm, unsigned Kind)
{
  const MCFixupKindInfo &FKI =
    Asm.getBackend().getFixupKindInfo((MCFixupKind) Kind);

  return FKI.Flags & MCFixupKindInfo::FKF_IsPCRel;
}

static const MCSymbol* transSymbolToReloc(const MCAssembler &Asm,
                                          const MCValue &Target,
                                          const MCFragment &F,
                                          const MCFixup &Fixup,
                                          bool IsPCRel,
                                          MCObjectReader* OR)
{
  const MCSymbol &Symbol = Target.getSymA()->getSymbol();
  const MCSymbol &ASymbol = Symbol.AliasedSymbol();
  const MCSymbolData &SD = Asm.getSymbolData(Symbol);

  if (ASymbol.isUndefined()) {
    return &ASymbol;
  }

  if (SD.isExternal()) {
    return &Symbol;
  }

  const MCSectionELF &Section =
    static_cast<const MCSectionELF&>(ASymbol.getSection());
  const SectionKind secKind = Section.getKind();

  if (secKind.isBSS())
    return OR->explicitRelSym(Asm, Target, F, Fixup, IsPCRel);

  if (secKind.isThreadLocal()) {
    return &Symbol;
  }

  MCSymbolRefExpr::VariantKind Kind = Target.getSymA()->getKind();
  const MCSectionELF &Sec2 =
    static_cast<const MCSectionELF&>(F.getParent()->getSection());

  if (&Sec2 != &Section &&
      (Kind == MCSymbolRefExpr::VK_PLT ||
       Kind == MCSymbolRefExpr::VK_GOTPCREL ||
       Kind == MCSymbolRefExpr::VK_GOTOFF)) {
    return &Symbol;
  }

  if (Section.getFlags() & ELF::SHF_MERGE) {
    if (Target.getConstant() == 0)
      return OR->explicitRelSym(Asm, Target, F, Fixup, IsPCRel);
    return &Symbol;
  }

  return OR->explicitRelSym(Asm, Target, F, Fixup, IsPCRel);
}


//===----------------------------------------------------------------------===//
// MCAsmObjectReader
MCAsmObjectReader::MCAsmObjectReader(MCObjectStreamer &pStreamer,
                                     TargetLDBackend& pBackend,
                                     MCLDInfo& pLDInfo)
  : MCObjectWriter(llvm::nulls(),
                   pStreamer.getAssembler().getWriter().isLittleEndian()),
    m_Backend(pBackend),
    m_LDInfo(pLDInfo) {
  pStreamer.getAssembler().setWriter(*this);
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
                                         const MCFixup &Fixup,
                                         MCValue Target,
                                         uint64_t &FixedValue)
{
  int64_t Addend = 0;
  int Index = 0;
  int64_t Value = Target.getConstant();
  const MCSymbol *RelocSymbol = NULL;

  bool IsPCRel = isFixupKindPCRel(Asm, Fixup.getKind());
  MCObjectReader* OR = m_Backend.getObjectReader();

  if (!Target.isAbsolute()) {
    const MCSymbol &Symbol = Target.getSymA()->getSymbol();
    const MCSymbol &ASymbol = Symbol.AliasedSymbol();
    RelocSymbol = transSymbolToReloc(Asm, Target, *Fragment, Fixup, IsPCRel, OR);

    if (const MCSymbolRefExpr* RefB = Target.getSymB()) {
      const MCSymbol& SymbolB = RefB->getSymbol();
      MCSymbolData& SDB = Asm.getSymbolData(SymbolB);
      IsPCRel = true;

      int64_t a = Layout.getSymbolOffset(&SDB); // symbol in section
      int64_t b = Layout.getFragmentOffset(Fragment) + Fixup.getOffset(); // relocation in section
      Value += b-a;
    }

    if (!RelocSymbol) {
      MCSymbolData& SD = Asm.getSymbolData(ASymbol);
      // Offset of the symbol in the section
      Value += Layout.getSymbolOffset(&SD);
    }
    Addend = Value;
  }

  FixedValue = Value; // parameter out
  unsigned Type = OR->getRelocType(Target, Fixup, IsPCRel, (RelocSymbol != 0), Addend);
  MCSymbolData& SD = Asm.getSymbolData(*RelocSymbol);

  if (!OR->hasRelocationAddend())
    Addend = 0;

  RelocationEntry RE(Layout.getFragmentOffset(Fragment),  // r_offset
                     Addend,  // r_addend
                     SD.Index << 8 + (unsigned char)Type, // r_info
                     &SD);  // MCSymbolData
  m_LDInfo.bitcode().context()->getRelocInfo().entries.push_back(RE);
}

void MCAsmObjectReader::WriteObject(MCAssembler &Asm,
                                    const MCAsmLayout &Layout)
{
}

