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

  void reserveLocalEntry();
  void reserveGlobalEntry();

  size_t getTotalNum() const;
  size_t getLocalNum() const;

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

public:
  /// Do real allocation of the GOT entries.
  virtual void finalizeSectionSize();

private:
  typedef llvm::DenseMap<const ResolveInfo*, bool> SymbolTypeMapType;

private:
  SymbolTypeMapType m_GOTTypeMap;

  size_t m_pLocalNum;           ///< number of reserved local entries
  size_t m_pGlobalNum;          ///< number of reserved global entries

  MipsGOTEntry* m_pLastLocal;   ///< the last consumed local entry
  MipsGOTEntry* m_pLastGlobal;  ///< the last consumed global entry

private:
  void reserve(size_t pNum);
};

} // namespace of mcld

#endif

