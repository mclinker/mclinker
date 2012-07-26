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
#include <mcld/LD/SectionData.h>

namespace mcld
{
class LDSection;
class MemoryRegion;

/** \class MipsGOT
 *  \brief Mips Global Offset Table.
 */
class MipsGOT : public GOT
{
private:
  typedef llvm::DenseMap<const ResolveInfo*, GOTEntry*> SymbolIndexMapType;
  typedef llvm::DenseMap<const ResolveInfo*, bool> SymbolTypeMapType;

public:
  typedef SectionData::iterator iterator;
  typedef SectionData::const_iterator const_iterator;

public:
  MipsGOT(LDSection& pSection, SectionData& pSectionData);

  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end() const;

  uint64_t emit(MemoryRegion& pRegion);

  void reserveLocalEntry();
  void reserveGlobalEntry();

  GOTEntry* getEntry(const ResolveInfo& pInfo, bool& pExist);

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
  SymbolIndexMapType m_GeneralGOTMap; // Map ResolveInfo* to GOTEntry *.
  SymbolTypeMapType m_GOTTypeMap;

  iterator m_LocalGOTIterator;  // last local GOT entries
  iterator m_GlobalGOTIterator; // last global GOT entries
  size_t m_pLocalNum;

private:
  // Use reserveLocalEntry()/reserveGlobalEntry() instead of this routine.
  void reserveEntry(size_t pNum = 1);
};

} // namespace of mcld

#endif

