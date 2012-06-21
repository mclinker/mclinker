//===- X86GOT.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_X86_GOT_H
#define MCLD_X86_GOT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "X86PLT.h"
#include <mcld/Target/GOT.h>

namespace mcld
{
class LDSection;

/** \class X86GOT
 *  \brief X86 Global Offset Table.
 */

class X86GOT : public GOT
{
  typedef llvm::DenseMap<const ResolveInfo*, GOTEntry*> SymbolIndexMapType;

public:
  typedef llvm::MCSectionData::iterator iterator;
  typedef llvm::MCSectionData::const_iterator const_iterator;

public:
  X86GOT(LDSection& pSection, llvm::MCSectionData& pSectionData);

  ~X86GOT();

  //Reserve general GOT entries.
  void reserveEntry(size_t pNum = 1);

  GOTEntry* getEntry(const ResolveInfo& pSymbol, bool& pExist);

  iterator begin();

  const_iterator begin() const;

  iterator end();

  const_iterator end() const;

private:
  /// m_GOTIterator - point to the first valid entry in GOT list
  iterator m_GOTIterator;

  /// m_fIsVisit - first time visit the function getEntry() or not
  bool m_fIsVisit;

  SymbolIndexMapType m_GOTMap;
};

} // namespace of mcld

#endif

