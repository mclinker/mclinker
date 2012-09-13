//===- impl.cpp -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ARMGOT.h"

#include <new>

#include <llvm/Support/Casting.h>

#include <mcld/LD/LDSection.h>
#include <mcld/LD/LDFileFormat.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MsgHandling.h>

namespace {
  const size_t ARMGOTEntrySize = 4;
  const unsigned int ARMGOT0Num = 3;
} // end of anonymous namespace

using namespace mcld;

//===----------------------------------------------------------------------===//
// ARMGOT
ARMGOT::ARMGOT(LDSection& pSection, SectionData& pSectionData)
  : GOT(pSection, pSectionData, ARMGOTEntrySize)
{
  // Create GOT0 entries.
  reserve(ARMGOT0Num);

  // Skip GOT0 entries.
  for (int i = 0; i < ARMGOT0Num; ++i) {
    consume();
  }
}

ARMGOT::~ARMGOT()
{
}

bool ARMGOT::hasGOT1() const
{
  return (m_SectionData.size() > ARMGOT0Num);
}

void ARMGOT::reserveGOTPLT()
{
  Entry* entry = new Entry(0, getEntrySize(), &m_SectionData);
  if (NULL == m_GOTPLT.front) {
    if (NULL == m_GOT.front) {
      // GOT part is also empty. Since entry is the last entry, we can assign
      // it to GOTPLT directly.
      m_GOTPLT.front = entry;
      m_GOTPLT.rear = entry;
    }
    else {
      // GOT part is not empty. Shift GOT backward by one entry.
      m_GOTPLT.front = m_GOT.front;
      m_GOTPLT.rear = m_GOT.front;

      m_GOT.front = llvm::cast<GOT::Entry>(m_GOT.front->getNextNode());
      m_GOT.rear = llvm::cast<GOT::Entry>(m_GOT.rear->getNextNode());
    }
  }
  else {
    if (NULL == m_GOT.front)
      m_GOTPLT.rear = entry;
    else {
      m_GOTPLT.rear = m_GOT.front;

      m_GOT.front = llvm::cast<GOT::Entry>(m_GOT.front->getNextNode());
      m_GOT.rear = llvm::cast<GOT::Entry>(m_GOT.rear->getNextNode());
    }
  }
}

void ARMGOT::reserveGOT()
{
  Entry* entry = new Entry(0, getEntrySize(), &m_SectionData);
  if (NULL == m_GOT.front) {
    // Entry must be the last entry. We can directly assign it to GOT part.
    m_GOT.front = entry;
  }
  m_GOT.rear = entry;
}

GOT::Entry* ARMGOT::consumeGOTPLT()
{
  assert(NULL != m_GOTPLT.front && NULL != m_GOTPLT.rear &&
         "Consuming empty GOTPLT section!");

  assert(m_GOTPLT.last_used != m_GOTPLT.rear && "No entry to consumed!");

  if (NULL == m_GOTPLT.last_used) {
    m_GOTPLT.last_used = m_GOTPLT.front;
  }
  else {
    m_GOTPLT.last_used = llvm::cast<GOT::Entry>(m_GOTPLT.last_used->getNextNode());
  }
  return m_GOTPLT.last_used;
}

GOT::Entry* ARMGOT::consumeGOT()
{
  assert(NULL != m_GOT.front && NULL != m_GOT.rear && "Consuming empty GOT section!");

  assert(m_GOT.last_used != m_GOT.rear && "No entry to consumed!");

  if (NULL == m_GOT.last_used) {
    m_GOT.last_used = m_GOT.front;
  }
  else {
    m_GOT.last_used = llvm::cast<GOT::Entry>(m_GOT.last_used->getNextNode());
  }
  return m_GOT.last_used;
}

void ARMGOT::applyGOT0(uint64_t pAddress)
{
  llvm::cast<Entry>
    (*(m_SectionData.getFragmentList().begin())).setContent(pAddress);
}

void ARMGOT::applyGOTPLT(uint64_t pPLTBase)
{
  if (NULL == m_GOTPLT.front || NULL == m_GOTPLT.rear)
    return;

  Entry* entry = m_GOTPLT.front;
  while (entry != m_GOTPLT.rear) {
    entry->setContent(pPLTBase);
    entry = llvm::cast<GOT::Entry>(entry->getNextNode());
  }

  m_GOTPLT.rear->setContent(pPLTBase);
}

uint64_t ARMGOT::emit(MemoryRegion& pRegion)
{
  uint32_t* buffer = reinterpret_cast<uint32_t*>(pRegion.getBuffer());

  Entry* got = 0;
  unsigned int entry_size = getEntrySize();
  uint64_t result = 0x0;
  for (iterator it = begin(), ie = end(); it != ie; ++it, ++buffer) {
      got = &(llvm::cast<Entry>((*it)));
      *buffer = static_cast<uint32_t>(got->getContent());
      result += entry_size;
  }
  return result;
}

