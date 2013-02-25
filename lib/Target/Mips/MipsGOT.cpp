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
#include <llvm/Support/ELF.h>

#include <mcld/LD/ResolveInfo.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Target/OutputRelocSection.h>

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
    m_GlobalNum(global),
    m_ConsumedLocal(0),
    m_ConsumedGlobal(0),
    m_pLastLocal(NULL),
    m_pLastGlobal(NULL)
{
}

bool MipsGOT::GOTMultipart::isConsumed() const
{
  return m_LocalNum == m_ConsumedLocal &&
         m_GlobalNum == m_ConsumedGlobal;
}

void MipsGOT::GOTMultipart::consumeLocal()
{
  assert(m_ConsumedLocal < m_LocalNum &&
         "Consumed too many local GOT entries");
  ++m_ConsumedLocal;
  m_pLastLocal = llvm::cast<MipsGOTEntry>(m_pLastLocal->getNextNode());
}

void MipsGOT::GOTMultipart::consumeGlobal()
{
  assert(m_ConsumedGlobal < m_GlobalNum &&
         "Consumed too many global GOT entries");
  ++m_ConsumedGlobal;
  m_pLastGlobal = llvm::cast<MipsGOTEntry>(m_pLastGlobal->getNextNode());
}

//===----------------------------------------------------------------------===//
// MipsGOT
//===----------------------------------------------------------------------===//
MipsGOT::MipsGOT(LDSection& pSection)
  : GOT(pSection),
    m_pInput(NULL),
    m_InputLocalNum(0),
    m_CurrentGOTPart(0),
    m_TotalLocalNum(0),
    m_TotalGlobalNum(0)
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
  return !m_MultipartList.empty();
}

void MipsGOT::finalizeScanning(OutputRelocSection& pRelDyn)
{
  m_TotalLocalNum  = 0;
  m_TotalGlobalNum = 0;

  for (MultipartListType::const_iterator it = m_MultipartList.begin();
       it != m_MultipartList.end(); ++it) {
    m_TotalLocalNum  += it->m_LocalNum;
    m_TotalGlobalNum += it->m_GlobalNum;
  }

  for (MultipartListType::iterator it = m_MultipartList.begin();
       it != m_MultipartList.end(); ++it) {
    it->m_pLastLocal = llvm::cast<MipsGOTEntry>(&m_SectionData->back());
    reserve(it->m_LocalNum);
    it->m_pLastGlobal = llvm::cast<MipsGOTEntry>(&m_SectionData->back());
    reserve(it->m_GlobalNum);

    if (it == m_MultipartList.begin())
      // Reserve entries in the second part of the primary GOT.
      // These entries correspond to the global symbols in all
      // non-primary GOTs.
      reserve(m_TotalGlobalNum - it->m_GlobalNum);
    else {
      // Reserver reldyn entries for R_MIPS_REL32 relocations
      // for all global entries of secondary GOTs.
      // FIXME: (simon) Do not count local entries for non-pic.
      size_t count = it->m_GlobalNum + it->m_LocalNum;
      for (size_t i = 0; i < count; ++i)
        pRelDyn.reserveEntry();
    }
  }
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

void MipsGOT::merge(const Input& pInput, const ResolveInfo* pInfo)
{
  if (m_pInput == NULL) {
    m_pInput = &pInput;
    m_InputLocalNum = 0;
    m_MultipartList.push_back(GOTMultipart());
  }

  if (m_pInput != &pInput) {
    m_pInput = &pInput;
    m_InputLocalNum = 0;
    m_InputGlobalSymbols.clear();
  }

  size_t gotSize = m_MultipartList.back().m_LocalNum +
                   m_MultipartList.back().m_GlobalNum;

  if (m_MultipartList.size() == 1)
    gotSize += MipsGOT0Num;

  assert(gotSize <= MipsGOTSize && "GOT overflow!");

  // GOT is not filled completely.
  if (gotSize < MipsGOTSize)
    return;

  // This global symbol has been counted already
  // and does not increases the GOT size.
  if (pInfo && m_MergedGlobalSymbols.count(pInfo))
    return;

  m_MultipartList.back().m_Inputs.erase(&pInput);
  m_MultipartList.back().m_LocalNum -= m_InputLocalNum;
  for (SymbolCountMapType::const_iterator it = m_InputGlobalSymbols.begin();
                                          it != m_InputGlobalSymbols.end();
                                          ++it) {
    SymbolCountMapType::iterator m = m_MergedGlobalSymbols.find(it->first);

    assert(m != m_MergedGlobalSymbols.end() && "Cannot find merged symbol");
    assert(m->second >= it->second && "Incorrect merged symbols number");

    if (m->second - it->second == 0)
      m_MergedGlobalSymbols.erase(m);
  }
  m_MultipartList.back().m_GlobalNum = m_MergedGlobalSymbols.size();

  m_MergedGlobalSymbols = m_InputGlobalSymbols;
  m_MultipartList.push_back(GOTMultipart(m_InputLocalNum,
                                         m_InputGlobalSymbols.size()));
}

bool MipsGOT::isReserved(const Input& pInput, const ResolveInfo& pInfo) const
{
  return (pInfo.reserved() & 2/*MipsGNULDBackend::ReserveGot*/) != 0;
}

void MipsGOT::reserveLocalEntry(const Input& pInput)
{
  merge(pInput, NULL);

  m_MultipartList.back().m_Inputs.insert(&pInput);
  ++m_MultipartList.back().m_LocalNum;
  ++m_InputLocalNum;
}

void MipsGOT::reserveGlobalEntry(const Input& pInput, const ResolveInfo& pInfo)
{
  merge(pInput, &pInfo);

  m_MultipartList.back().m_Inputs.insert(&pInput);
  ++m_MultipartList.back().m_GlobalNum;
  ++m_MergedGlobalSymbols[&pInfo];
  ++m_InputGlobalSymbols[&pInfo];
}

bool MipsGOT::isPrimaryGOTConsumed()
{
  return m_CurrentGOTPart > 0;
}

MipsGOTEntry* MipsGOT::consumeLocal()
{
  assert(m_CurrentGOTPart < m_MultipartList.size() && "GOT number is out of range!");

  if (m_MultipartList[m_CurrentGOTPart].isConsumed())
    ++m_CurrentGOTPart;

  m_MultipartList[m_CurrentGOTPart].consumeLocal();

  return m_MultipartList[m_CurrentGOTPart].m_pLastLocal;
}

MipsGOTEntry* MipsGOT::consumeGlobal()
{
  assert(m_CurrentGOTPart < m_MultipartList.size() && "GOT number is out of range!");

  if (m_MultipartList[m_CurrentGOTPart].isConsumed())
    ++m_CurrentGOTPart;

  m_MultipartList[m_CurrentGOTPart].consumeGlobal();

  return m_MultipartList[m_CurrentGOTPart].m_pLastGlobal;
}

uint64_t MipsGOT::getGPAddr(Input& pInput)
{
  size_t gotNum = 0;
  for (size_t i = 0; i < m_MultipartList.size(); ++i) {
    if (m_MultipartList[i].m_Inputs.count(&pInput)) {
      gotNum = i;
      break;
    }
  }

  return addr() + 0x7FF0 * (2 * gotNum + 1);
}

void MipsGOT::recordEntry(const Input* pInput,
                          const ResolveInfo* pInfo,
                          MipsGOTEntry* pEntry)
{
  GotEntryKey key;
  key.m_pInput = pInput;
  key.m_pInfo = pInfo;
  m_GotEntriesMap[key] = pEntry;
}

MipsGOTEntry* MipsGOT::lookupEntry(const Input* pInput,
                                   const ResolveInfo* pInfo)
{
  GotEntryKey key;
  key.m_pInput = pInput;
  key.m_pInfo = pInfo;
  GotEntryMapType::iterator it = m_GotEntriesMap.find(key);

  if (it == m_GotEntriesMap.end())
    return NULL;

  return it->second;
}

size_t MipsGOT::getLocalNum() const
{
  assert(!m_MultipartList.empty() && "GOT is empty!");
  return m_MultipartList[0].m_LocalNum + MipsGOT0Num;
}

size_t MipsGOT::getGlobalNum() const
{
  return m_TotalGlobalNum;
}
