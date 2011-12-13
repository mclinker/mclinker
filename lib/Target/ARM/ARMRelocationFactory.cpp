//===- ARMRelocationFactory.cpp  ------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/Support/ELF.h>
#include <llvm/ADT/StringRef.h>
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

void ARMRelocationFactory::apply(Relocation& Relocation)
{
   Pointer perform = m_ApplyFuncs[Relocation.type()];
   ARM_Reloc_Status stts = (this->*perform)(Relocation);
   switch (stts) {
     default: {
       llvm_unreachable("Why are you here?");
     }
     case STATUS_OK: {
       break;
     }
     case STATUS_OVERFLOW: {
       llvm::report_fatal_error(llvm::StringRef("Relocation overflow on ") +
                                Relocation.symInfo()->name());
       break;
     }
     case STATUS_BAD_RELOC: {
       llvm::report_fatal_error(llvm::StringRef("Unexpected opcode while "
                                "applying relocation on ") +
                                Relocation.symInfo()->name());
       break;
     }
   }
}

ARMRelocationFactory::Pointer ARMRelocationFactory::m_ApplyFuncs[]=
{
                                          // Code   Name
                                          // --------------------------
  &ARMRelocationFactory::none,            // 0      R_ARM_NONE/
                                          //        Unsupport/Deprecated
  &ARMRelocationFactory::none,            // 1      R_ARM_PC24
  &ARMRelocationFactory::abs32,           // 2      R_ARM_ABS32
  &ARMRelocationFactory::rel32,           // 3      R_ARM_REL32
  &ARMRelocationFactory::none,            // 4
  &ARMRelocationFactory::none,            // 5
  &ARMRelocationFactory::none,            // 6
  &ARMRelocationFactory::none,            // 7
  &ARMRelocationFactory::none,            // 8
  &ARMRelocationFactory::none,            // 9
  &ARMRelocationFactory::thm_call,        // 10     R_ARM_THM_CALL
  &ARMRelocationFactory::none,            // 11
  &ARMRelocationFactory::none,            // 12
  &ARMRelocationFactory::none,            // 13
  &ARMRelocationFactory::none,            // 14
  &ARMRelocationFactory::none,            // 15
  &ARMRelocationFactory::none,            // 16
  &ARMRelocationFactory::none,            // 17
  &ARMRelocationFactory::none,            // 18
  &ARMRelocationFactory::none,            // 19
  &ARMRelocationFactory::none,            // 20
  &ARMRelocationFactory::none,            // 21
  &ARMRelocationFactory::none,            // 22
  &ARMRelocationFactory::none,            // 23
  &ARMRelocationFactory::gotoff32,        // 24     R_ARM_GOTOFF32
  &ARMRelocationFactory::none,            // 25
  &ARMRelocationFactory::got_brel,        // 26     R_ARM_GOT_BREL
  &ARMRelocationFactory::none,            // 27     R_ARM_PLT32
  &ARMRelocationFactory::call,            // 28     R_ARM_CALL
  &ARMRelocationFactory::none,            // 29
  &ARMRelocationFactory::jump24,          // 30     R_ARM_JUMP24
  &ARMRelocationFactory::none,            // 31
  &ARMRelocationFactory::none,            // 32
  &ARMRelocationFactory::none,            // 33
  &ARMRelocationFactory::none,            // 34
  &ARMRelocationFactory::none,            // 35
  &ARMRelocationFactory::none,            // 36
  &ARMRelocationFactory::none,            // 37
  &ARMRelocationFactory::none,            // 38
  &ARMRelocationFactory::none,            // 39
  &ARMRelocationFactory::none,            // 40
  &ARMRelocationFactory::none,            // 41
  &ARMRelocationFactory::none,            // 42
  &ARMRelocationFactory::movw_abs_nc,     // 43     R_ARM_MOVW_ABS_NC
  &ARMRelocationFactory::movt_abs,        // 44     R_ARM_MOVT_ABS
  &ARMRelocationFactory::movw_prel_nc,    // 45     R_ARM_MOVW_PREL_NC
  &ARMRelocationFactory::movt_prel,       // 46     R_ARM_MOVT_PREL
  &ARMRelocationFactory::thm_movw_abs_nc, // 47     R_ARM_THM_MOVW_ABS_NC
  &ARMRelocationFactory::thm_movt_abs,    // 48     R_ARM_THM_MOVT_ARBS
  &ARMRelocationFactory::thm_movw_prel_nc,// 49     R_ARM_THM_MOVW_PREL_NC
  &ARMRelocationFactory::thm_movt_prel,   // 50     R_ARM_THM_MOVT_PREL
  &ARMRelocationFactory::none,            // 104    R_ARM_TLS_GD32
  &ARMRelocationFactory::none,            // 107    R_ARM_TLS_IE32
  &ARMRelocationFactory::none             // 108    R_ARM_TLS_LE32
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
ARM_Reloc_Status ARMRelocationFactory::none(Relocation& pReloc)
{
  return STATUS_OK;
}

// R_ARM_ABS32: (S + A) | T
ARM_Reloc_Status ARMRelocationFactory::abs32(Relocation& pReloc)
{
  DWord t_bit = thumbBit(pReloc);
  DWord addend = pReloc.target() + pReloc.addend();
  pReloc.target() = (pReloc.symValue() + addend) | t_bit;
  return STATUS_OK;
}

// R_ARM_REL32: ((S + A) | T) - P
ARM_Reloc_Status ARMRelocationFactory::rel32(Relocation& pReloc)
{
  DWord t_bit = thumbBit(pReloc);
  DWord addend = pReloc.target() + pReloc.addend();
  pReloc.target() = ((pReloc.symValue() + addend) | t_bit)
                    - pReloc.place(*layout());
  return STATUS_OK;
}

// R_ARM_THM_CALL: ((S + A) | T) - P
ARM_Reloc_Status ARMRelocationFactory::thm_call(Relocation& pReloc)
{
  return STATUS_OK;
}

// R_ARM_GOTOFF32: ((S + A) | T) - GOT_ORG
ARM_Reloc_Status ARMRelocationFactory::gotoff32(Relocation& pReloc)
{
  DWord t_bit = thumbBit(pReloc);
  DWord addend = pReloc.target() + pReloc.addend();
  pReloc.target() = ((pReloc.symValue() + addend) | t_bit) - gotorg();
  return STATUS_OK;
}

// R_ARM_GOT_BREL: GOT(S) + A - GOT_ORG
ARM_Reloc_Status ARMRelocationFactory::got_brel(Relocation& pReloc)
{
  return STATUS_OK;
}

// R_ARM_CALL: ((S + A) | T) - P
ARM_Reloc_Status ARMRelocationFactory::call(Relocation& pReloc)
{
  return STATUS_OK;
}

// R_ARM_JUMP24: ((S + A) | T) - P
ARM_Reloc_Status ARMRelocationFactory::jump24(Relocation& pReloc)
{
  return STATUS_OK;
}

// R_ARM_MOVW_ABS_NC: (S + A) | T
ARM_Reloc_Status ARMRelocationFactory::movw_abs_nc(Relocation& pReloc)
{
  return STATUS_OK;
}

// R_ARM_MOVT_ABS: S + A
ARM_Reloc_Status ARMRelocationFactory::movt_abs(Relocation& pReloc)
{
  return STATUS_OK;
}

// R_ARM_MOVW_PREL_NC: ((S + A) | T) - P
ARM_Reloc_Status ARMRelocationFactory::movw_prel_nc(Relocation& pReloc)
{
  return STATUS_OK;
}

// R_ARM_MOVT_PREL: S + A - P
ARM_Reloc_Status ARMRelocationFactory::movt_prel(Relocation& pReloc)
{
  return STATUS_OK;
}

// R_ARM_THM_MOVW_ABS_NC: (S + A) | T
ARM_Reloc_Status ARMRelocationFactory::thm_movw_abs_nc(Relocation& pReloc)
{
  return STATUS_OK;
}

// R_ARM_THM_MOVT_ABS: S + A
ARM_Reloc_Status ARMRelocationFactory::thm_movt_abs(Relocation& pReloc)
{
  return STATUS_OK;
}

// R_ARM_THM_MOVW_PREL_NC: ((S + A) | T) - P
ARM_Reloc_Status ARMRelocationFactory::thm_movw_prel_nc(Relocation& pReloc)
{
  return STATUS_OK;
}

// R_ARM_THM_MOVT_PREL: S + A - P
ARM_Reloc_Status ARMRelocationFactory::thm_movt_prel(Relocation& pReloc)
{
  return STATUS_OK;
}
