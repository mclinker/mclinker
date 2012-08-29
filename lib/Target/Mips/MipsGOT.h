//===- MipsGOT.h ----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_MIPS_GOT_H
#define MCLD_MIPS_GOT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/ADT/DenseMap.h>

#include <mcld/Target/GOT.h>

namespace mcld
{
class LDSection;
class MemoryRegion;

/** \class MipsGOT
 *  \brief Mips Global Offset Table.
 */
class MipsGOT : public GOT
{
public:
  MipsGOT(LDSection& pSection, SectionData& pSectionData);

  uint64_t emit(MemoryRegion& pRegion);

  void reserveLocalEntry();
  void reserveGlobalEntry();

  /// getOrConsumeEntry - override parant's function, we need to maintain the
  /// Global and Local iterator here
  GOTEntry* getOrConsumeEntry(const ResolveInfo& pInfo, bool& pExist);

  size_t getTotalNum() const;
  size_t getLocalNum() const;

  void setLocal(const ResolveInfo* pInfo) {
    m_GOTTypeMap[pInfo] = false;
  }

  void setGlobal(const ResolveInfo* pInfo) {
    m_GOTTypeMap[pInfo] = true;
  }

  bool isLocal(const ResolveInfo* pInfo) {
    return m_GOTTypeMap[pInfo] == false;
  }

  bool isGlobal(const ResolveInfo* pInfo) {
    return m_GOTTypeMap[pInfo] == true;
  }

private:
  typedef llvm::DenseMap<const ResolveInfo*, bool> SymbolTypeMapType;

private:
  SymbolTypeMapType m_GOTTypeMap;

  iterator m_LocalGOTIterator;  // last local GOT entries
  iterator m_GlobalGOTIterator; // last global GOT entries
  size_t m_pLocalNum;
};

} // namespace of mcld

#endif

