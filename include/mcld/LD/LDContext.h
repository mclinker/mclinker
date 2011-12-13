//===- LDContext.h --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LDCONTEXT_H
#define MCLD_LDCONTEXT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "llvm/MC/MCAssembler.h"
#include <vector>


using namespace llvm;

namespace mcld
{

/** \class LDContext
 *  \brief LDContext stores the data which a object file should has
 */
class LDContext
{
public:
  typedef std::vector<llvm::MCSectionData*> SectionTable;
  typedef SectionTable::iterator sect_iterator;
  typedef SectionTable::const_iterator const_sect_iterator;

public:
  LDContext();

  ~LDContext();

  SectionTable& getSectionTable()
  { return m_SectionTable; }

  const SectionTable& getSectionTable() const
  { return m_SectionTable; }

  sect_iterator begin()
  { return m_SectionTable.begin(); }

  sect_iterator end()
  { return m_SectionTable.end(); }

  const_sect_iterator begin() const
  { return m_SectionTable.begin(); }

  const_sect_iterator end() const
  { return m_SectionTable.end(); }

  size_t numOfSections() const
  { return m_SectionTable.size(); }

private:
  SectionTable m_SectionTable;

};

} // namespace of mcld

#endif

