//===- ARMRelocationFactory.cpp  ------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/Support/ELF.h>
#include "ARMRelocationFactory.h"

using namespace mcld;

//==========================
// ARMRelocationFactory
ARMRelocationFactory::ARMRelocationFactory(size_t pNum) :
                                           RelocationFactory(pNum)
{
}

ARMRelocationFactory::~ARMRelocationFactory()
{
}

void ARMRelocationFactory::apply(Relocation& Relocation)
{
   Pointer perform = m_ApplyFuncs[Relocation.type()];
   (this->*perform)(Relocation);
}

ARMRelocationFactory::Pointer ARMRelocationFactory::m_ApplyFuncs[]=
{
                                        // Code   Name
                                        // --------------------------
  &ARMRelocationFactory::none,          // 0      R_ARM_NONE
  &ARMRelocationFactory::none,          // 1      R_ARM_PC24
  &ARMRelocationFactory::abs32,         // 2      R_ARM_ABS32
  &ARMRelocationFactory::rel32          // 3      R_ARM_REL32
  // TODO:
};

RelocationFactory::DWord ARMRelocationFactory::thumbBit(Relocation& pReloc)
{
  // Set thumb bit if
  // - symbol has type of STT_ARM_TFUNC (?) or
  // - symbol has type of STT_FUNC, is defined and with bit 0 of its value set
  DWord thumbBit =                          //FIXME: check symbol is not undef
        (((pReloc.symInfo()->type() == llvm::ELF::STT_FUNC) &&
        ((pReloc.symInfo()->value()&1) != 0))
        ? 1:0 ) ;
  return thumbBit;
}

// R_ARM_NONE and those unsupported/deprecated relocation type
void ARMRelocationFactory::none(Relocation& pReloc)
{
  return;
}

//R_ARM_ABS32: (S + A) | T
void ARMRelocationFactory::abs32(Relocation& pReloc)
{
  DWord t_bit = thumbBit(pReloc);
  DWord addend = pReloc.target() + pReloc.addend();
  pReloc.target() = (pReloc.symValue() + addend) | t_bit;
}

//R_ARM_REL32: ((S + A) | T) - P
void ARMRelocationFactory::rel32(Relocation& pReloc)
{
  DWord t_bit = thumbBit(pReloc);
  DWord addend = pReloc.target() + pReloc.addend();
  pReloc.target() = ((pReloc.symValue() + addend) | t_bit)
                    - pReloc.place(*layout());
}

//R_ARM_GOTOFF32: ((S + A) | T) - GOT_ORG
void ARMRelocationFactory::gotoff32(Relocation& pReloc)
{
  DWord t_bit = thumbBit(pReloc);
  DWord addend = pReloc.target() + pReloc.addend();
  pReloc.target() = ((pReloc.symValue() + addend) | t_bit) - gotorg();
}

//R_ARM_GOT_BREL: GOT(S) + A - GOT_ORG
void ARMRelocationFactory::got_brel(Relocation& pReloc)
{
}

