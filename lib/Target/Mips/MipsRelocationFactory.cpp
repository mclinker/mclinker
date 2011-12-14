//===- MipsRelocationFactory.cpp  -----------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/ADT/Twine.h>
#include <llvm/Support/ELF.h>
#include <llvm/Support/ErrorHandling.h>
#include "MipsRelocationFactory.h"

using namespace mcld;

//==========================
// MipsRelocationFactory
MipsRelocationFactory::MipsRelocationFactory(size_t pNum)
  : RelocationFactory(pNum)
{
}

MipsRelocationFactory::~MipsRelocationFactory()
{
}

MipsRelocationFactory::Pointer MipsRelocationFactory::m_ApplyFuncs[]=
{
#ifdef MIPS_RELOC_FUNC
# error "MIPS_RELOC_FUNC should be undefined."
#else
# define MIPS_RELOC_FUNC(FuncName, Num, Name) \
    &MipsRelocationFactory::FuncName,
# include "MipsRelocationFunction.def"
#endif
};

void MipsRelocationFactory::apply(Relocation& Relocation)
{
  Relocation::Type type = Relocation.type();

  if (type >= sizeof(m_ApplyFuncs) / sizeof(m_ApplyFuncs[0])) {
    llvm::report_fatal_error(llvm::Twine("Unexpected relocation type while "
                                         "applying relocation on ") +
                             Relocation.symInfo()->name());
  }

  Pointer perform = m_ApplyFuncs[type];
  MipsRelocStatus stts = (this->*perform)(Relocation);
  switch (stts) {
    default: {
      llvm_unreachable("Unknown relocation status");
    }
    case STATUS_OK: {
      break;
    }
    case STATUS_OVERFLOW: {
      llvm::report_fatal_error(llvm::Twine("Relocation overflow on ") +
                               Relocation.symInfo()->name());
      break;
    }
    case STATUS_BAD_RELOC: {
      llvm::report_fatal_error(llvm::Twine("Unexpected opcode while "
                                           "applying relocation on ") +
                               Relocation.symInfo()->name());
      break;
    }
  }
}

//=========================================//
// Relocation functions implementation     //
//=========================================//

// R_MIPS_NONE and those unsupported/deprecated relocation type
MipsRelocStatus MipsRelocationFactory::none(Relocation& pReloc)
{
  return STATUS_OK;
}
