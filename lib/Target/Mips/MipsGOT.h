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

/** \class MipsGOTEntry
 *  \brief GOT Entry with size of 4 bytes
 */
class MipsGOTEntry : public GOT::Entry<4>
{
public:
  MipsGOTEntry(uint64_t pContent, SectionData* pParent)
   : GOT::Entry<4>(pContent, pParent)
  {}
};

/** \class MipsGOT
 *  \brief Mips Global Offset Table.
 */
class MipsGOT : public GOT
{
public:
  MipsGOT(LDSection& pSection);

  uint64_t emit(MemoryRegion& pRegion);

  void reserve(size_t pNum = 1);
  void reserveLocalEntry();
  void reserveGlobalEntry();

  size_t getTotalNum() const;
  size_t getLocalNum() const;

  MipsGOTEntry* consume();
  MipsGOTEntry* consumeLocal();
  MipsGOTEntry* consumeGlobal();

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

  /// hasGOT1 - return if this got section has any GOT1 entry
  bool hasGOT1() const;

private:
  typedef llvm::DenseMap<const ResolveInfo*, bool> SymbolTypeMapType;

private:
  SymbolTypeMapType m_GOTTypeMap;

  iterator m_LocalGOTIterator;  // last local GOT entries
  iterator m_GlobalGOTIterator; // last global GOT entries
  size_t m_pLocalNum;

  MipsGOTEntry* m_pLast; ///< the last consumed entry
};

} // namespace of mcld

#endif

