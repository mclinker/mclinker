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

#include <mcld/Target/GOT.h>

namespace mcld
{

class LDSection;

/** \class MipsGOT
 *  \brief Mips Global Offset Table.
 */
class MipsGOT : public GOT
{
private:
  typedef llvm::DenseMap<const ResolveInfo*, GOTEntry*> SymbolIndexMapType;

public:
  typedef llvm::MCSectionData::iterator iterator;
  typedef llvm::MCSectionData::const_iterator const_iterator;

public:
  MipsGOT(LDSection& pSection, llvm::MCSectionData& pSectionData);

  // While reserveEntry, the size in LDSection is also updated. As the result,
  // layout will get the correct size of GOT section
  void reserveEntry(int pNum = 1);

  GOTEntry* getEntry(const ResolveInfo& pInfo, bool& pExist);

  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end() const;

private:
  SymbolIndexMapType m_GeneralGOTMap;
  iterator m_GeneralGOTIterator;
};

} // namespace of mcld

#endif

