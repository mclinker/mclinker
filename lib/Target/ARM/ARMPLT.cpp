//===- ARMPLT.cpp -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ARMGOT.h"
#include "ARMPLT.h"
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/ErrorHandling.h>

using namespace mcld;

namespace {

const uint32_t arm_plt0[] = {
  0xe52de004, // str   lr, [sp, #-4]!
  0xe59fe004, // ldr   lr, [pc, #4]
  0xe08fe00e, // add   lr, pc, lr
  0xe5bef008, // ldr   pc, [lr, #8]!
  0x00000000, // &GOT[0] - .
};

const uint32_t arm_plt1[] = {
  0xe28fc600, // add   ip, pc, #0xNN00000
  0xe28cca00, // add   ip, ip, #0xNN000
  0xe5bcf000, // ldr   pc, [ip, #0xNNN]!
};

}

ARMPLT0::ARMPLT0() : PLTEntry(sizeof(arm_plt0)) {}
ARMPLT1::ARMPLT1() : PLTEntry(sizeof(arm_plt1)) {}

//===----------------------------------------------------------------------===//
// ARMPLT

ARMPLT::ARMPLT(llvm::MCSectionData* pSectionData, ARMGOT &pGOT)
  : PLT(pSectionData), m_GOT(pGOT), iter()
{
  ARMPLT0* plt0_entry = new ARMPLT0();
  m_pSectionData->getFragmentList().push_back(plt0_entry);

  //TODO: Check whether iterator will get the end() here, and
  //      then stay at the end(). If it will, this will be buggy.
  iter = pSectionData->begin();
}

ARMPLT::~ARMPLT()
{
}

void ARMPLT::reserveEntry(int pNum)
{
  ARMPLT1* plt1_entry = new ARMPLT1();
  m_pSectionData->getFragmentList().push_back(plt1_entry);

  GOTEntry* got_entry= new GOTEntry(0);
  m_GOT.GOTPLTEntries->getEntryList().push_back(got_entry);
}

PLTEntry* ARMPLT::getEntry(const ResolveInfo& pSymbol, bool& pExist)
{
   ARMPLT1 *&Entry = m_SymbolIndexMap[&pSymbol];
   pExist = 1;

   if (!Entry) {
     pExist = 0;

     assert(iter != m_pSectionData->end() &&
            "The number of PLT Entries and ResolveInfo doesn't match");

     ++iter; // This will skip PLT0.
     Entry = llvm::cast<ARMPLT1>(&(*iter));
   }

   return Entry;
}

void ARMPLT::applyPLT0(const uint32_t pOffset) {

  MCFragmentIterator first = m_pSectionData->getFragmentList().begin();
  ARMPLT0* plt0 = &(llvm::cast<ARMPLT0>(*first));

  uint32_t* data = 0;
  data = static_cast<uint32_t*>(malloc(plt0->getEntrySize()));

  if (!data)
    llvm::report_fatal_error("Allocating new memory for plt0 failed!");

  memcpy(data, arm_plt0, plt0->getEntrySize());
  data[4] = pOffset;

  plt0->setContent(reinterpret_cast<unsigned char*>(data));
}

void ARMPLT::applyPLT1(const uint32_t pOffset, const ResolveInfo& pInfo) {
  ARMPLT1* plt1 = m_SymbolIndexMap[&pInfo];

  if (plt1) {
    uint32_t* Out = 0;
    Out = static_cast<uint32_t*>(malloc(plt1->getEntrySize()));

    if (!Out)
      llvm::report_fatal_error("Allocating new memory for plt1 failed!");

    memcpy(Out, arm_plt1, plt1->getEntrySize());
    uint32_t offset = pOffset;

    uint32_t plt1_data1 = ((offset-8) & 0x0FF00000);
    uint32_t plt1_data2 = ((offset-4) & 0x000FF000);
    uint32_t plt1_data3 = (offset & 0x00000FFF);

    *Out |= plt1_data1;
    *Out |= plt1_data2;
    *Out |= plt1_data3;

    plt1->setContent(reinterpret_cast<unsigned char*>(Out));
  }

  llvm::errs()<<"Can not resolve non-exist PLT1 entry!\n";
}
