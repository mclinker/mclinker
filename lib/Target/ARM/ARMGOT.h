//===- ARMGOT.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ARM_GOT_H
#define MCLD_ARM_GOT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "ARMPLT.h"
#include "mcld/Target/GOT.h"

namespace mcld
{

class LDSection;

// GOT entries on ARM can be arranged into three categories,
// these three categories are reserved GOT entries, got.plt
// entries, and general GOT entries. A ARMGOTEntries keeps
// only one kind of them to avoid mixing all kinds of GOT entries.
class ARMGOTEntries : public llvm::MCFragment {
public:
  // iplist<MCFragment> here is used to keep instances of GOTEntry class
  typedef llvm::iplist<MCFragment> GOTEntryListType;

  typedef GOTEntryListType::const_iterator const_iterator;
  typedef GOTEntryListType::iterator iterator;

  typedef GOTEntryListType::const_reverse_iterator const_reverse_iterator;
  typedef GOTEntryListType::reverse_iterator reverse_iterator;

public:
  void addEntry(MCFragment& Entry) { EntryList.push_back(&Entry); }

  GOTEntryListType& getEntryList() { return EntryList; }
  const GOTEntryListType& getEntryList() const {return EntryList; }

  iterator begin() { return EntryList.begin(); }
  const_iterator got_begin() const { return EntryList.begin(); }

  iterator end() { return EntryList.end(); }
  const_iterator const got_end() { return EntryList.end(); }

  reverse_iterator rbegin() { return EntryList.rbegin(); }
  const_reverse_iterator rbegin() const { return EntryList.rbegin(); }

  reverse_iterator rend() { return EntryList.rend(); }
  const_reverse_iterator rend() const { return EntryList.rend(); }

  size_t size() const { return EntryList.size(); }

  bool empty() const { return EntryList.empty(); }

private:
  GOTEntryListType EntryList;
};

/** \class ARMGOT
 *  \brief ARM Global Offset Table.
 */
class ARMGOT : public GOT
{
friend void mcld::ARMPLT::reserveEntry(int pNum);

typedef ARMGOTEntries::iterator GOTIteratorType;
typedef llvm::DenseMap<const ResolveInfo*, GOTEntry*> SymbolIndexMapType;

public:
  ARMGOT(llvm::MCSectionData& pSectionData);

  ~ARMGOT();

  //Reserve general GOT entries.
  void reserveEntry(int pNum = 1);

  GOTEntry* getEntry(const ResolveInfo* pSymbol, bool& pExist);

  void applyGOT0(uint64_t pAddress);

  void applyGOTPLT(uint64_t pAddress);

private:

  //Keep first three GOT entries in .got section.
  ARMGOTEntries* m_pGOT0Entries;

  ARMGOTEntries* m_pGOTPLTEntries;

  ARMGOTEntries* m_pGeneralGOTEntries;

  GOTIteratorType m_GeneralGOTIterator;

  SymbolIndexMapType m_SymbolIndexMap;
};

} // namespace of mcld

#endif

