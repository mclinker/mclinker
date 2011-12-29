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
#include <new>

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

namespace mcld {

ARMPLT0::ARMPLT0() : PLTEntry(sizeof(arm_plt0)) {}
ARMPLT1::ARMPLT1() : PLTEntry(sizeof(arm_plt1)) {}

//===----------------------------------------------------------------------===//
// ARMPLT

ARMPLT::ARMPLT(LDSection& pSection,
               llvm::MCSectionData& pSectionData,
               ARMGOT &pGOTPLT)
  : PLT(pSection, pSectionData), m_GOTPLT(pGOTPLT), m_PLTEntryIterator() {
  ARMPLT0* plt0_entry = new ARMPLT0();
  pSectionData.getFragmentList().push_back(plt0_entry);

  m_PLTEntryIterator = pSectionData.begin();
}

ARMPLT::~ARMPLT()
{
}

void ARMPLT::reserveEntry(int pNum)
{
  ARMPLT1* plt1_entry = 0;
  GOTEntry* got_entry = 0;

  for (int i = 0; i < pNum; i++) {
    plt1_entry = new (std::nothrow) ARMPLT1();

    if (!plt1_entry)
      llvm::report_fatal_error("Allocating new memory for ARMPLT1 failed!");

    m_pSectionData->getFragmentList().push_back(plt1_entry);

    got_entry= new (std::nothrow) GOTEntry(0, m_GOTPLT.getEntrySize());

    if (!got_entry)
      llvm::report_fatal_error("Allocating new memory for GOT failed!");

    m_GOTPLT.m_SectionData.getFragmentList().push_back(got_entry);

    ++(m_GOTPLT.m_GOTPLTNum);
    ++(m_GOTPLT.m_GeneralGOTIterator);
  }
}

PLTEntry* ARMPLT::getPLTEntry(const ResolveInfo& pSymbol, bool& pExist)
{
   ARMPLT1 *&PLTEntry = m_PLTEntryMap[&pSymbol];

   pExist = 1;

   if (!PLTEntry) {
     GOTEntry *&GOTPLTEntry = m_GOTPLT.m_GOTPLTMap[&pSymbol];
     assert(!GOTPLTEntry && "PLT entry and got.plt entry doesn't match!");

     pExist = 0;

     // This will skip PLT0.
     assert((++m_PLTEntryIterator) != m_pSectionData->end() &&
            "The number of PLT Entries and ResolveInfo doesn't match");
     ++(m_GOTPLT.m_GOTPLTIterator);

     PLTEntry = llvm::cast<ARMPLT1>(&(*m_PLTEntryIterator));
     GOTPLTEntry = llvm::cast<GOTEntry>(&(*(m_GOTPLT.m_GOTPLTIterator)));
   }

   return PLTEntry;
}

GOTEntry* ARMPLT::getGOTPLTEntry(const ResolveInfo& pSymbol, bool& pExist)
{
   GOTEntry *&GOTPLTEntry = m_GOTPLT.m_GOTPLTMap[&pSymbol];

   pExist = 1;

   if (!GOTPLTEntry) {
     ARMPLT1 *&PLTEntry = m_PLTEntryMap[&pSymbol];
     assert(!PLTEntry && "PLT entry and got.plt entry doesn't match!");

     pExist = 0;

     // This will skip PLT0.
     assert((++m_PLTEntryIterator) != m_pSectionData->end() &&
            "The number of PLT Entries and ResolveInfo doesn't match");
     ++(m_GOTPLT.m_GOTPLTIterator);

     PLTEntry = llvm::cast<ARMPLT1>(&(*m_PLTEntryIterator));
     GOTPLTEntry = llvm::cast<GOTEntry>(&(*(m_GOTPLT.m_GOTPLTIterator)));
   }

   return GOTPLTEntry;
}

ARMPLT0* ARMPLT::getPLT0() const {
  if (!m_pSectionData)
    llvm::report_fatal_error("m_pSectionData is NULL!");

  iterator first = m_pSectionData->getFragmentList().begin();
  iterator end = m_pSectionData->getFragmentList().end();

  assert(first!=end && "FragmentList is empty, getPLT0 failed!");

  ARMPLT0* plt0 = &(llvm::cast<ARMPLT0>(*first));

  return plt0;
}

void ARMPLT::applyPLT0() {

  if (!m_pSectionData)
    llvm::report_fatal_error("m_pSectionData is NULL!");

  uint64_t plt_base =
    llvm::cast<LDSection>(m_pSectionData->getSection()).offset();
  assert(plt_base && ".plt base address is NULL!");

  uint64_t got_base =
    llvm::cast<LDSection>(m_GOTPLT.getSectionData().getSection()).offset();
  assert(got_base && ".got base address is NULL!");

  uint32_t offset = 0;

  if (got_base > plt_base)
    offset = got_base - (plt_base + 16);
  else
    offset = (plt_base + 16) - got_base;

  iterator first = m_pSectionData->getFragmentList().begin();
  iterator end = m_pSectionData->getFragmentList().end();

  assert(first!=end && "FragmentList is empty, applyPLT0 failed!");

  ARMPLT0* plt0 = &(llvm::cast<ARMPLT0>(*first));

  uint32_t* data = 0;
  data = static_cast<uint32_t*>(malloc(plt0->getEntrySize()));

  if (!data)
    llvm::report_fatal_error("Allocating new memory for plt0 failed!");

  memcpy(data, arm_plt0, plt0->getEntrySize());
  data[4] = offset;

  plt0->setContent(reinterpret_cast<unsigned char*>(data));
}

void ARMPLT::applyPLT1() {

  uint64_t plt_base =
    llvm::cast<LDSection>(m_pSectionData->getSection()).offset();
  assert(plt_base && ".plt base address is NULL!");

  uint64_t got_base =
    llvm::cast<LDSection>(m_GOTPLT.getSectionData().getSection()).offset();
  assert(got_base && ".got base address is NULL!");

  ARMPLT::iterator it = m_pSectionData->begin();
  ARMPLT::iterator ie = m_pSectionData->end();
  assert(it!=ie && "FragmentList is empty, applyPLT1 failed!");

  uint64_t GOTEntrySize = m_GOTPLT.getEntrySize();
  uint64_t GOTEntryAddress =
    got_base +  GOTEntrySize * 3;

  uint64_t PLTEntryAddress =
    plt_base + llvm::cast<ARMPLT0>((*it)).getEntrySize() + 8; //Offset of PLT0

  ++it; //skip PLT0
  uint64_t PLT1EntrySize = llvm::cast<ARMPLT1>((*it)).getEntrySize();
  ARMPLT1* plt1 = 0;

  unsigned int EntryCounter = 0;
  uint64_t Offset = 0;

  while (it != ie) {
    plt1 = &(llvm::cast<ARMPLT1>(*it));
    uint32_t* Out = 0;
    Out = static_cast<uint32_t*>(malloc(plt1->getEntrySize()));

    if (!Out)
      llvm::report_fatal_error("Allocating new memory for plt1 failed!");

    memcpy(Out, arm_plt1, plt1->getEntrySize());

    GOTEntryAddress = GOTEntryAddress + GOTEntrySize;
    PLTEntryAddress = PLTEntryAddress + PLT1EntrySize;

    if (GOTEntryAddress > PLTEntryAddress)
      Offset = GOTEntryAddress - PLTEntryAddress;
    else
      Offset = PLTEntryAddress - GOTEntryAddress;

    uint32_t plt1_data1 = ((Offset-8) & 0x0FF00000);
    uint32_t plt1_data2 = ((Offset-4) & 0x000FF000);
    uint32_t plt1_data3 = (Offset & 0x00000FFF);

    *Out |= plt1_data1;
    *Out |= plt1_data2;
    *Out |= plt1_data3;

    plt1->setContent(reinterpret_cast<unsigned char*>(Out));
  }

  ARMGOT::const_iterator
  gotplt_it = m_GOTPLT.getSectionData().getFragmentList().begin(),
  gotplt_ie = m_GOTPLT.getSectionData().getFragmentList().end();

  // Skip GOT0
  ++gotplt_it;
  ++gotplt_it;
  ++gotplt_it;

  for (; gotplt_it != gotplt_ie; ++gotplt_it)
    llvm::cast<GOTEntry>((*it)).setContent(plt_base);
}

} // end namespace mcld

