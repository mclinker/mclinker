//===- PLT.h --------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef PROCEDURE_LINKAGE_TABLE_H
#define PROCEDURE_LINKAGE_TABLE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/LD/LDSection.h"
#include <llvm/MC/MCAssembler.h>
#include <llvm/ADT/ilist.h>

namespace mcld
{

class GOT;
class LDSymbol;

/** \class PLTEntry
 */
class PLTEntry : public llvm::MCFragment
{
public:
  PLTEntry(unsigned int size, unsigned char* content);
  virtual ~PLTEntry();

  unsigned int getEntrySize() const {
    return m_EntrySize;
  }
  unsigned char* getContent() const {
    return m_pContent;
  }

protected:
  virtual void initPLTEntry() = 0;

protected:
  unsigned int m_EntrySize;
  unsigned char* m_pContent;
};

/** \class PLT
 *  \brief Procedure linkage table
 */
class PLT : public llvm::MCSectionData
{
public:
  typedef llvm::iplist<llvm::MCFragment> EntryListType;

public:
  PLT(const LDSection& pSection, GOT& pGOTPLT);
  virtual ~PLT();

  // -----  modifiers  ----- //
  virtual void addEntry(LDSymbol& pSymbol) = 0;

  // -----  observers  ----- //
  EntryListType& getEntryList()
  { return m_EntryList; }

  const EntryListType& getEntryList() const
  { return m_EntryList; }

  unsigned int entryCount() const
  { return m_EntryList.size(); }

private:
  EntryListType m_EntryList;
  GOT& m_GOTPLT;
};

} // namespace of mcld

#endif
