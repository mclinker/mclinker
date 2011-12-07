/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_GOT_H
#define MCLD_GOT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/MC/MCAssembler.h>

namespace mcld
{

/** \class GOT
 *  \brief Global Offset Table
 */
class GOT : public llvm::MCFragment
{
public:
  GOT(unsigned int pEntrySize);
  ~GOT();

  /** void addEntry(); **/

  unsigned int entrySize() const
  { return m_EntrySize; }

  unsigned int entryNum() const
  { return m_EntryNum; }

  static bool classof(llvm::MCFragment* F)
  { return (F->getKind() == llvm::MCFragment::FT_GOT); }

  static bool classof(GOT* F)
  { return true; }

private:
  unsigned int m_EntrySize;
  unsigned int m_EntryNum;
  unsigned char* m_Table; // gold uses vector<Got_entry>
};

} // namespace of mcld

#endif

