//===- MCBitcodeInterceptor.cpp -------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/Relocation.h>
#include <mcld/MC/MCBitcodeInterceptor.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/Target/TargetLDBackend.h>
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
#include <llvm/Support/ELF.h>

using namespace llvm;
using namespace mcld;

//===----------------------------------------------------------------------===//
// non-member functions

//===----------------------------------------------------------------------===//
// MCBitcodeInterceptor
MCBitcodeInterceptor::MCBitcodeInterceptor(MCObjectStreamer &pStreamer,
                                           TargetLDBackend& pBackend,
                                           MCLDInfo& pLDInfo)
  : MCObjectWriter(llvm::nulls(),
                   pStreamer.getAssembler().getWriter().isLittleEndian()),
    m_Backend(pBackend),
    m_LDInfo(pLDInfo) {
  pStreamer.getAssembler().setWriter(*this);
}

MCBitcodeInterceptor::~MCBitcodeInterceptor()
{
}

void MCBitcodeInterceptor::ExecutePostLayoutBinding(MCAssembler &Asm,
                                                 const MCAsmLayout &Layout)
{
}


void MCBitcodeInterceptor::RecordRelocation(const MCAssembler &Asm,
                                         const MCAsmLayout &Layout,
                                         const MCFragment *Fragment,
                                         const MCFixup &Fixup,
                                         MCValue Target,
                                         uint64_t &FixedValue)
{
}

/// WriteObject - not really write out a object. Instead, load data to LDContext
void MCBitcodeInterceptor::WriteObject(MCAssembler &Asm,
                                       const MCAsmLayout &Layout)
{
}


