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
class ResolveInfo;

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
  typedef GOT GOTPLTType;

public:
  PLT(const LDSection& pSection, GOT& pGOTPLT);
  virtual ~PLT();

  GOTPLTType& getGOTPLT()
  { return m_GOTPLT; }

  const GOTPLTType& getGOTPLT() const
  { return m_GOTPLT; }

public:
  virtual PLTEntry* getOrCreateGOTPLT(const ResolveInfo& info) = 0;

protected:
  GOTPLTType& m_GOTPLT;
};

} // namespace of mcld

#endif
