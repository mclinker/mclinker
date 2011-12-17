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

/** \class GOTEntry
 *  \brief The entry of Global Offset Table
 */
class GOTEntry : public llvm::MCFragment
{
public:
  GOTEntry(unsigned int size ,
           unsigned char* content);

  virtual ~GOTEntry();

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

/** \class GOT
 *  \brief The Global Offset Table
 */
class GOT
{
protected:
  GOT(const std::string pSectionName);

public:
  virtual ~GOT();

  LDSection* getSection();

  const LDSection* getSection() const;

protected:
  LDSection m_Section;
};

} // namespace of mcld

#endif

