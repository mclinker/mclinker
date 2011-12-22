//===- GOT.cpp ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/LDSection.h>
#include <mcld/Target/GOT.h>
#include <cstring>
#include <cstdlib>

using namespace mcld;

//===----------------------------------------------------------------------===//
// GOTEntry
GOTEntry::GOTEntry(uint64_t pContent)
  : llvm::MCFragment(llvm::MCFragment::FT_GOT, 0),
    f_Content(pContent) {
}

GOTEntry::~GOTEntry()
{
}

//===----------------------------------------------------------------------===//
// GOT
GOT::GOT(const LDSection& pSection, unsigned int pEntryBytes)
  : llvm::MCSectionData(pSection),
    f_EntryBytes(pEntryBytes) {
}

GOT::~GOT()
{
}

unsigned int GOT::entryBytes() const
{
  return f_EntryBytes;
}

