/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.lu@mediatek.com>                                          *
 ****************************************************************************/
#ifndef PROCEDURE_LINKAGE_TABLE_H
#define PROCEDURE_LINKAGE_TABLE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
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
  PLTEntry();
  virtual ~PLTEntry();

  PLTEntry* cloneFirstEntry() const
  { return doCloneFirstEntry(); }

  PLTEntry* cloneNormalEntry() const
  { return doCloneNormalEntry(); }

  /**
  PLTEntry* cloneTLSDescEntry() const
  { return doTLSDescEntry(); }
  **/

  unsigned char* getContent() const
  { return doGetContent(); }

  static bool classof(const MCFragment* F)
  { return F->getKind() == llvm::MCFragment::FT_PLT; }

  static bool classof(const Entry*)
  { return true; }

private:
  virtual PLTEntry* doCloneFirstEntry() = 0;
  virtual PLTEntry* doCloneNormalEntry() = 0;
  // virtual PLTEntry* doCloneTLSDescEntry() = 0;
  virtual unsigned char* doGetContent() = 0;

};

/** \class PLT
 *  \brief Procedure linkage table
 */
class PLT 
{
public:
  typedef iplist<PLTEntry> EntryListType;

public:
  PLT(GOT& pGOT);
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
  GOT& m_GOT;

};

} // namespace of mcld

#endif

