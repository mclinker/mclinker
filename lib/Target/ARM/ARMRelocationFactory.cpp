//===- ARMRelocationFactory.cpp  ------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <string>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/ErrorHandling.h>
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

void ARMRelocationFactory::apply(Relocation& pRelocation)
{
  Relocation::Type type = pRelocation.type();
  if (type > 130) { // 131-255 doesn't noted in ARM spec
    llvm::report_fatal_error(llvm::Twine("Unexpected relocation type while "
                             "applying relocation on ") +
                             pRelocation.symInfo()->name());
  }
  Pointer perform = m_ApplyFuncs[type];
  ARMRelocStatus stts = (this->*perform)(pRelocation);
  switch (stts) {
    default: {
      llvm_unreachable("Why are you here?");
    }
    case STATUS_OK: {
      break;
    }
    case STATUS_OVERFLOW: {
      llvm::report_fatal_error(llvm::Twine("Relocation overflow on ") +
                               pRelocation.symInfo()->name());
      break;
    }
    case STATUS_BAD_RELOC: {
      llvm::report_fatal_error(llvm::Twine("Unexpected opcode while "
                               "applying relocation on ") +
                               pRelocation.symInfo()->name());
      break;
    }
  }
}

ARMRelocationFactory::Pointer ARMRelocationFactory::m_ApplyFuncs[]=
{
#ifdef ARM_RELOC_FUNC
# error "ARM_RELOC_FUNC should be undefined."
#else
# define ARM_RELOC_FUNC(FuncName, Num, Name) \
    &ARMRelocationFactory::FuncName,
# include "ARMRelocationFunction.def"
#endif
};

RelocationFactory::DWord ARMRelocationFactory::getThumbBit(Relocation& pReloc)
{
  // Set thumb bit if
  // - symbol has type of STT_ARM_TFUNC (?) or
  // - symbol has type of STT_FUNC, is defined and with bit 0 of its value set
  DWord thumbBit =                          //FIXME: check symbol is not undef
        (((pReloc.symInfo()->type() == ResolveInfo::Function) &&
        ((pReloc.symInfo()->value()&1) != 0))
        ? 1:0 ) ;
  return thumbBit;
}


//=========================================//
// Each relocation function implementation //
//=========================================//

// R_ARM_NONE and those unsupported/deprecated relocation type
ARMRelocStatus ARMRelocationFactory::none(Relocation& pReloc)
{
  return STATUS_OK;
}

// R_ARM_ABS32: (S + A) | T
ARMRelocStatus ARMRelocationFactory::abs32(Relocation& pReloc)
{
  DWord t_bit = getThumbBit(pReloc);
  DWord addend = pReloc.target() + pReloc.addend();
  pReloc.target() = (pReloc.symValue() + addend) | t_bit;
  return STATUS_OK;
}

// R_ARM_REL32: ((S + A) | T) - P
ARMRelocStatus ARMRelocationFactory::rel32(Relocation& pReloc)
{
  DWord t_bit = getThumbBit(pReloc);
  DWord addend = pReloc.target() + pReloc.addend();
  pReloc.target() = ((pReloc.symValue() + addend) | t_bit)
                    - pReloc.place(*layout());
  return STATUS_OK;
}

// R_ARM_GOTOFF32: ((S + A) | T) - GOT_ORG
ARMRelocStatus ARMRelocationFactory::gotoff32(Relocation& pReloc)
{
  DWord t_bit = getThumbBit(pReloc);
  DWord addend = pReloc.target() + pReloc.addend();
  pReloc.target() = ((pReloc.symValue() + addend) | t_bit) - gotorg();
  return STATUS_OK;
}
