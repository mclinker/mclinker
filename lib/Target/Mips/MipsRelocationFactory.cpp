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
#include "MipsRelocationFunctions.h"

using namespace mcld;

DECL_MIPS_APPLY_RELOC_FUNCS

//==========================
// MipsRelocationFactory
MipsRelocationFactory::MipsRelocationFactory(size_t pNum, MipsGNULDBackend& pParent)
  : RelocationFactory(pNum, pParent)
{
}

MipsRelocationFactory::~MipsRelocationFactory()
{
}

void MipsRelocationFactory::applyRelocation(Relocation& pRelocation)
{
  Relocation::Type type = pRelocation.type();

  /// the prototype of applying function
  /* Can Mips relocation functions get all resources from such kind of function prototype?
   * If not, please add new parameters or change to non-constant MipsRelocationFactory.
   */
  typedef Result (*ApplyFunctionType)(Relocation&, const MipsRelocationFactory& );

  // the table entry of applying functions
  struct ApplyFunctionTriple {
    ApplyFunctionType func;
    unsigned int type;
    const char* name;
  };

  // declare the table of applying functions
  static ApplyFunctionTriple apply_functions[] = {
    DECL_MIPS_APPLY_RELOC_FUNC_PTRS
  };

  if (type >= sizeof(apply_functions) / sizeof(apply_functions[0])) {
    llvm::report_fatal_error(llvm::Twine("Unknown relocation type. To symbol `") +
                             pRelocation.symInfo()->name() +
                             llvm::Twine("'."));
  }

  // apply the relocation 
  Result result = apply_functions[type].func(pRelocation, *this);

  // check result
  if (Overflow == result) {
    llvm::report_fatal_error(llvm::Twine("Applying relocation `") +
                             llvm::Twine(apply_functions[type].name) +
                             llvm::Twine("' causes overflow. on symbol: `") +
                             llvm::Twine(pRelocation.symInfo()->name()) +
                             llvm::Twine("'."));
    return;
  }

  if (BadReloc == result) {
    llvm::report_fatal_error(llvm::Twine("Applying relocation `") +
                             llvm::Twine(apply_functions[type].name) +
                             llvm::Twine("' encounters unexpected opcode. on symbol: `") +
                             llvm::Twine(pRelocation.symInfo()->name()) +
                             llvm::Twine("'."));
    return;
  }
}

//=========================================//
// Relocation functions implementation     //
//=========================================//

// R_MIPS_NONE and those unsupported/deprecated relocation type
MipsRelocationFactory::Result
none(Relocation& pReloc, const MipsRelocationFactory& pParent)
{
  return MipsRelocationFactory::OK;
}

