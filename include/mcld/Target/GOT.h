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
#include <llvm/MC/MCAssembler.h>
#include <mcld/LD/LDSection.h>

namespace mcld
{

class GOT;

/** \class GOTEntry
 *  \brief The entry of Global Offset Table
 */
class GOTEntry : public llvm::MCFragment
{
public:
  explicit GOTEntry(uint64_t pContent, GOT* pParent);

  virtual ~GOTEntry();

  uint64_t& getContent()
  { return f_Content; }

  const uint64_t& getContent() const
  { return f_Content; }

protected:
  uint64_t f_Content;
};

/** \class GOT
 *  \brief The Global Offset Table
 */
class GOT : public llvm::MCSectionData
{
protected:
  GOT(const LDSection& pSection, unsigned int pEntrySize);

public:
  virtual ~GOT();

  /// entrySize - the number of bytes per entry
  unsigned int entryBytes() const;

  virtual GOTEntry* createEntry(uint64_t pData = 0) = 0;

protected:
  unsigned int f_EntryBytes;
};

} // namespace of mcld

#endif

