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

namespace mcld {

//===----------------------------------------------------------------------===//
// non-member functions
static bool isFixupKindPCRel(const llvm::MCAssembler &Asm, unsigned Kind)
{
  const llvm::MCFixupKindInfo &FKI =
    Asm.getBackend().getFixupKindInfo((llvm::MCFixupKind) Kind);

  return FKI.Flags & llvm::MCFixupKindInfo::FKF_IsPCRel;
}

//===----------------------------------------------------------------------===//
// MCAsmObjectReader
MCAsmObjectReader::MCAsmObjectReader(llvm::MCObjectStreamer &pStreamer,
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

void MCAsmObjectReader::ExecutePostLayoutBinding(llvm::MCAssembler &Asm,
                                                 const llvm::MCAsmLayout &Layout)
{
}


void MCAsmObjectReader::RecordRelocation(const llvm::MCAssembler &Asm,
                                         const llvm::MCAsmLayout &Layout,
                                         const llvm::MCFragment *Fragment,
                                         const llvm::MCFixup &Fixup,
                                         llvm::MCValue Target,
                                         uint64_t &FixedValue)
{
  int64_t Addend = 0;
  int Index = 0;
  int64_t Value = Target.getConstant();
  const llvm::MCSymbol *RelocSymbol = NULL;

  bool IsPCRel = isFixupKindPCRel(Asm, Fixup.getKind());
  MCObjectReader* OR = m_Backend.getObjectReader();

  if (!Target.isAbsolute()) {
    const llvm::MCSymbol &Symbol = Target.getSymA()->getSymbol();
    const llvm::MCSymbol &ASymbol = Symbol.AliasedSymbol();
    //RelocSymbol = transSymbolToReloc(Asm, Target, *Fragment, Fixup, IsPCRel, OR);

    if (const llvm::MCSymbolRefExpr* RefB = Target.getSymB()) {
      const llvm::MCSymbol& SymbolB = RefB->getSymbol();
      llvm::MCSymbolData& SDB = Asm.getSymbolData(SymbolB);
      IsPCRel = true;

      int64_t a = Layout.getSymbolOffset(&SDB); // symbol in section
      int64_t b = Layout.getFragmentOffset(Fragment) + Fixup.getOffset(); // relocation in section
      Value += b-a;
    }

    if (!RelocSymbol) {
      llvm::MCSymbolData& SD = Asm.getSymbolData(ASymbol);
      // Offset of the symbol in the section
      Value += Layout.getSymbolOffset(&SD);
    }
    Addend = Value;
  }

  FixedValue = Value; // parameter out
  unsigned Type = 0;//OR->getRelocType(Target, Fixup, IsPCRel, (RelocSymbol != 0), Addend);
  llvm::MCSymbolData& SD = Asm.getSymbolData(*RelocSymbol);

  //if (!OR->hasRelocationAddend())
  //  Addend = 0;

  //RelocationEntry RE(Layout.getFragmentOffset(Fragment),  // r_offset
  //                   Addend,  // r_addend
  //                   SD.Index << 8 + (unsigned char)Type, // r_info
  //                   &SD);  // MCSymbolData
  //m_LDInfo.bitcode().context()->getRelocInfo().entries.push_back(RE);
}

void MCAsmObjectReader::WriteObject(llvm::MCAssembler &Asm,
                                    const llvm::MCAsmLayout &Layout)
{
}

} //end namespace mcld
