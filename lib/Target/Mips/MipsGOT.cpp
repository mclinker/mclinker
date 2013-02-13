//===- MipsGOT.cpp --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MipsGOT.h"

#include <llvm/Support/Casting.h>

#include <mcld/LD/ResolveInfo.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MsgHandling.h>

namespace {
  const size_t MipsGOT0Num = 1;
  const size_t MipsGOTSize = 0xffff / mcld::MipsGOTEntry::EntrySize;
}

using namespace mcld;

//===----------------------------------------------------------------------===//
// MipsGOT::GOTMultipart
//===----------------------------------------------------------------------===//
MipsGOT::GOTMultipart::GOTMultipart(size_t local, size_t global)
  : m_LocalNum(local),
    m_GlobalNum(global)
{
}

//===----------------------------------------------------------------------===//
// MipsGOT
//===----------------------------------------------------------------------===//
MipsGOT::MipsGOT(LDSection& pSection)
  : GOT(pSection),
    m_pInput(NULL),
    m_LocalNum(0),
    m_GlobalNum(0),
    m_pLastLocal(NULL),
    m_pLastGlobal(NULL)
{
  // Create GOT0 entries.
  reserve(MipsGOT0Num);
}

void MipsGOT::reserve(size_t pNum)
{
  for (size_t i = 0; i < pNum; i++) {
    new MipsGOTEntry(0, m_SectionData);
  }
}

bool MipsGOT::hasGOT1() const
{
  return (m_LocalNum + m_GlobalNum) > 0;
}

void MipsGOT::finalizeSectionSize()
{
  m_pLastLocal = llvm::cast<MipsGOTEntry>(&m_SectionData->back());
  reserve(m_LocalNum);

  m_pLastGlobal = llvm::cast<MipsGOTEntry>(&m_SectionData->back());
  reserve(m_GlobalNum);

  GOT::finalizeSectionSize();
}

uint64_t MipsGOT::emit(MemoryRegion& pRegion)
{
  uint32_t* buffer = reinterpret_cast<uint32_t*>(pRegion.getBuffer());

  uint64_t result = 0;
  for (iterator it = begin(), ie = end();
       it != ie; ++it, ++buffer) {
    MipsGOTEntry* got = &(llvm::cast<MipsGOTEntry>((*it)));
    *buffer = static_cast<uint32_t>(got->getValue());
    result += got->size();
  }
  return result;
}

void MipsGOT::merge(const Input& pInput)
{
  if (m_pInput == NULL) {
    m_pInput = &pInput;
    m_CurrentGOT.m_LocalNum = 0;
    m_CurrentGOT.m_GlobalNum = 0;
    m_MultipartList.push_back(GOTMultipart());
  }

  if (m_pInput != &pInput) {
    m_pInput = &pInput;
    m_CurrentGOT.m_LocalNum = 0;
    m_CurrentGOT.m_GlobalNum = 0;
    m_InputGlobalSymbols.clear();
  }

  size_t gotSize = m_MultipartList.back().m_LocalNum +
                   m_MultipartList.back().m_GlobalNum;

  if (m_MultipartList.size() == 1)
    ++gotSize;

  assert(gotSize <= MipsGOTSize && "GOT overflow!");

  if (gotSize >= MipsGOTSize) {
    m_MultipartList.back().m_LocalNum -= m_CurrentGOT.m_LocalNum;
    m_MultipartList.back().m_GlobalNum -= m_CurrentGOT.m_GlobalNum;
    m_MergedGlobalSymbols = m_InputGlobalSymbols;
    m_MultipartList.push_back(GOTMultipart(m_CurrentGOT.m_LocalNum, m_CurrentGOT.m_GlobalNum));
  }
}

void MipsGOT::reserveLocalEntry(const Input& pInput)
{
  merge(pInput);

  ++m_MultipartList.back().m_LocalNum;
  ++m_CurrentGOT.m_LocalNum;

  ++m_LocalNum;
}

void MipsGOT::reserveGlobalEntry(const Input& pInput, const ResolveInfo& pInfo)
{
  merge(pInput);

  if (m_MergedGlobalSymbols.insert(&pInfo).second)
    ++m_MultipartList.back().m_GlobalNum;
  if (m_InputGlobalSymbols.insert(&pInfo).second)
    ++m_CurrentGOT.m_GlobalNum;

  ++m_GlobalNum;
}

MipsGOTEntry* MipsGOT::consumeLocal()
{
  assert(m_LocalNum && "Consume empty local GOT entry!");
  m_pLastLocal = llvm::cast<MipsGOTEntry>(m_pLastLocal->getNextNode());
  return m_pLastLocal;
}

MipsGOTEntry* MipsGOT::consumeGlobal()
{
  assert(m_GlobalNum && "Consume empty global GOT entry!");
  m_pLastGlobal = llvm::cast<MipsGOTEntry>(m_pLastGlobal->getNextNode());
  return m_pLastGlobal;
}

size_t MipsGOT::getTotalNum() const
{
  return getLocalNum() + m_GlobalNum;
}

size_t MipsGOT::getLocalNum() const
{
  return MipsGOT0Num + m_LocalNum;
}

