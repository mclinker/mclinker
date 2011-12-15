//===- GOT.h --------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_GOT_H
#define MCLD_GOT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include "llvm/MC/MCAssembler.h"

namespace mcld
{



/** \class GOT
 *  \brief Global Offset Table
 */
class GOTEntry : public llvm::MCFragment
{
public:
  GOTEntry(unsigned int size ,
           unsigned char* content);

  virtual ~GOTEntry();

  static bool classof(llvm::MCFragment* F)
  { return (F->getKind() == llvm::MCFragment::FT_GOT); }

  static bool classof(GOTEntry* F)
  { return true; }

  unsigned int getEntrySize() const {
    return m_EntrySize;
  }

  unsigned char* getContent() const {
    return m_pContent;
  }

protected:
  virtual void initGOTEntry() = 0;

protected:
  unsigned int m_EntrySize;
  unsigned char* m_pContent;
};

} // namespace of mcld

#endif

