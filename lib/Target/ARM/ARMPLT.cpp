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

#include <new>

#include <llvm/Support/Casting.h>

#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MsgHandling.h>

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

} // anonymous namespace

using namespace mcld;

ARMPLT0::ARMPLT0(SectionData* pParent)
  : PLTEntry(sizeof(arm_plt0), pParent) {}

ARMPLT1::ARMPLT1(SectionData* pParent)
  : PLTEntry(sizeof(arm_plt1), pParent) {}

//===----------------------------------------------------------------------===//
// ARMPLT

ARMPLT::ARMPLT(LDSection& pSection,
               SectionData& pSectionData,
               ARMGOT &pGOTPLT)
  : PLT(pSection, pSectionData), m_GOT(pGOTPLT), m_PLTEntryIterator() {
  ARMPLT0* plt0_entry = new ARMPLT0(&m_SectionData);

  m_Section.setSize(m_Section.size() + plt0_entry->getEntrySize());

  m_PLTEntryIterator = pSectionData.begin();
}

ARMPLT::~ARMPLT()
{
}

void ARMPLT::reserveEntry(size_t pNum)
{
  ARMPLT1* plt1_entry = 0;

  for (size_t i = 0; i < pNum; ++i) {
    plt1_entry = new (std::nothrow) ARMPLT1(&m_SectionData);

    if (!plt1_entry)
      fatal(diag::fail_allocate_memory_plt);

    m_Section.setSize(m_Section.size() + plt1_entry->getEntrySize());

    m_GOT.reserveGOTPLTEntry();
  }
}

PLTEntry* ARMPLT::getPLTEntry(const ResolveInfo& pSymbol, bool& pExist)
{
   ARMPLT1 *&PLTEntry = m_PLTEntryMap[&pSymbol];

   pExist = 1;

   if (!PLTEntry) {
     GOTEntry *&GOTPLTEntry = m_GOT.lookupGOTPLTMap(pSymbol);
     assert(!GOTPLTEntry && "PLT entry and got.plt entry doesn't match!");

     pExist = 0;

     // This will skip PLT0.
     ++m_PLTEntryIterator;
     assert(m_PLTEntryIterator != m_SectionData.end() &&
            "The number of PLT Entries and ResolveInfo doesn't match");

     ARMGOT::iterator got_it = m_GOT.getNextGOTPLTEntry();
     assert(got_it != m_GOT.getGOTPLTEnd() && "The number of GOTPLT and PLT doesn't match");

     PLTEntry = llvm::cast<ARMPLT1>(&(*m_PLTEntryIterator));
     GOTPLTEntry = llvm::cast<GOTEntry>(&(*got_it));
   }

   return PLTEntry;
}

GOTEntry* ARMPLT::getGOTPLTEntry(const ResolveInfo& pSymbol, bool& pExist)
{
   GOTEntry *&GOTPLTEntry = m_GOT.lookupGOTPLTMap(pSymbol);

   pExist = 1;

   if (!GOTPLTEntry) {
     ARMPLT1 *&PLTEntry = m_PLTEntryMap[&pSymbol];
     assert(!PLTEntry && "PLT entry and got.plt entry doesn't match!");

     pExist = 0;

     // This will skip PLT0.
     ++m_PLTEntryIterator;
     assert(m_PLTEntryIterator != m_SectionData.end() &&
            "The number of PLT Entries and ResolveInfo doesn't match");

     ARMGOT::iterator got_it = m_GOT.getNextGOTPLTEntry();
     assert(got_it != m_GOT.getGOTPLTEnd() &&
            "The number of GOTPLT and PLT doesn't match");

     PLTEntry = llvm::cast<ARMPLT1>(&(*m_PLTEntryIterator));
     GOTPLTEntry = llvm::cast<GOTEntry>(&(*got_it));
   }

   return GOTPLTEntry;
}

ARMPLT0* ARMPLT::getPLT0() const {

  iterator first = m_SectionData.getFragmentList().begin();

  assert(first != m_SectionData.getFragmentList().end() &&
         "FragmentList is empty, getPLT0 failed!");

  ARMPLT0* plt0 = &(llvm::cast<ARMPLT0>(*first));

  return plt0;
}

void ARMPLT::applyPLT0() {

  uint64_t plt_base = m_Section.addr();
  assert(plt_base && ".plt base address is NULL!");

  uint64_t got_base = m_GOT.getSection().addr();
  assert(got_base && ".got base address is NULL!");

  uint32_t offset = 0;

  if (got_base > plt_base)
    offset = got_base - (plt_base + 16);
  else
    offset = (plt_base + 16) - got_base;

  iterator first = m_SectionData.getFragmentList().begin();

  assert(first != m_SectionData.getFragmentList().end() &&
         "FragmentList is empty, applyPLT0 failed!");

  ARMPLT0* plt0 = &(llvm::cast<ARMPLT0>(*first));

  uint32_t* data = 0;
  data = static_cast<uint32_t*>(malloc(plt0->getEntrySize()));

  if (!data)
    fatal(diag::fail_allocate_memory_plt);

  memcpy(data, arm_plt0, plt0->getEntrySize());
  data[4] = offset;

  plt0->setContent(reinterpret_cast<unsigned char*>(data));
}

void ARMPLT::applyPLT1() {

  uint64_t plt_base = m_Section.addr();
  assert(plt_base && ".plt base address is NULL!");

  uint64_t got_base = m_GOT.getSection().addr();
  assert(got_base && ".got base address is NULL!");

  ARMPLT::iterator it = m_SectionData.begin();
  ARMPLT::iterator ie = m_SectionData.end();
  assert(it != ie && "FragmentList is empty, applyPLT1 failed!");

  uint32_t GOTEntrySize = m_GOT.getEntrySize();
  uint32_t GOTEntryAddress =
    got_base +  GOTEntrySize * 3;

  uint64_t PLTEntryAddress =
    plt_base + llvm::cast<ARMPLT0>((*it)).getEntrySize(); //Offset of PLT0

  ++it; //skip PLT0
  uint64_t PLT1EntrySize = llvm::cast<ARMPLT1>((*it)).getEntrySize();
  ARMPLT1* plt1 = NULL;

  uint32_t* Out = NULL;
  while (it != ie) {
    plt1 = &(llvm::cast<ARMPLT1>(*it));
    Out = static_cast<uint32_t*>(malloc(plt1->getEntrySize()));

    if (!Out)
      fatal(diag::fail_allocate_memory_plt);

    // Offset is the distance between the last PLT entry and the associated
    // GOT entry.
    int32_t Offset = (GOTEntryAddress - (PLTEntryAddress + 8));

    Out[0] = arm_plt1[0] | ((Offset >> 20) & 0xFF);
    Out[1] = arm_plt1[1] | ((Offset >> 12) & 0xFF);
    Out[2] = arm_plt1[2] | (Offset & 0xFFF);

    plt1->setContent(reinterpret_cast<unsigned char*>(Out));
    ++it;

    GOTEntryAddress += GOTEntrySize;
    PLTEntryAddress += PLT1EntrySize;
  }

  m_GOT.applyAllGOTPLT(plt_base);
}

uint64_t ARMPLT::emit(MemoryRegion& pRegion)
{
  uint64_t result = 0x0;
  iterator it = begin();
  unsigned int plt0_size = llvm::cast<ARMPLT0>((*it)).getEntrySize();

  unsigned char* buffer = pRegion.getBuffer();
  memcpy(buffer, llvm::cast<ARMPLT0>((*it)).getContent(), plt0_size);
  result += plt0_size;
  ++it;

  ARMPLT1* plt1 = 0;
  ARMPLT::iterator ie = end();
  unsigned int entry_size = 0;
  while (it != ie) {
    plt1 = &(llvm::cast<ARMPLT1>(*it));
    entry_size = plt1->getEntrySize();
    memcpy(buffer + result, plt1->getContent(), entry_size);
    result += entry_size;
    ++it;
  }
  return result;
}

