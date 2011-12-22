//===- impl.cpp -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ARMGOT.h"
#include "mcld/LD/LDFileFormat.h"
#include <llvm/Support/ErrorHandling.h>
#include <new>

namespace mcld {

//===----------------------------------------------------------------------===//
// ARMGOT
ARMGOT::ARMGOT(const LDSection& pSection)
  : GOT0Entries(new (std::nothrow) ARMGOTEntries),
    GOTPLTEntries(new (std::nothrow) ARMGOTEntries),
    GeneralGOTEntries(new (std::nothrow) ARMGOTEntries),
    GOT(pSection, 4) // ARM uses 32-bit GOT entry
{
  Fragments.push_back(GOT0Entries);
  Fragments.push_back(GOTPLTEntries);
  Fragments.push_back(GeneralGOTEntries);

  // Create GOT0 entries.
  GOTEntry* Entry1 = new (std::nothrow) GOTEntry(0);
  GOT0Entries->getEntryList().push_back(Entry1);

  GOTEntry* Entry2 = new (std::nothrow) GOTEntry(0);
  GOT0Entries->getEntryList().push_back(Entry2);

  GOTEntry* Entry3 = new (std::nothrow) GOTEntry(0);
  GOT0Entries->getEntryList().push_back(Entry3);
}

ARMGOT::~ARMGOT()
{
}

void ARMGOT::reserveEntry(int pNum)
{

  if (!GOT0Entries)
    llvm::report_fatal_error("GOT0Entries is NULL! "
                             "Allocating new memory in constructor failed" );

  GOTEntry* Entry = new (std::nothrow) GOTEntry(0);

  if (!Entry)
    llvm::report_fatal_error("Allocating new memory for GOTEntry failed");

  GOT0Entries->getEntryList().push_back(Entry);
}


GOTEntry* ARMGOT::getEntry(const ResolveInfo& pSymbol, bool& pExist)
{
  // TODO
  return 0;
}

} //end mcld namespace
