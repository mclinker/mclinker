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

#include <vector>
#include <mcld/LD/LDFileFormat.h>
#include <llvm/Support/DataTypes.h>
#include <string>
#include <cassert>

namespace mcld
{

class LDSymbol;
class LDSection;

/** \class LDContext
 *  \brief LDContext stores the data which a object file should has
 */
class LDContext
{
public:
  typedef std::vector<LDSection*> SectionTable;
  typedef SectionTable::iterator sect_iterator;
  typedef SectionTable::const_iterator const_sect_iterator;

  typedef std::vector<LDSymbol*> SymbolTable;
  typedef SymbolTable::iterator sym_iterator;
  typedef SymbolTable::const_iterator const_sym_iterator;

public:
  LDContext();

  ~LDContext();

  // -----  sections  ----- //
  SectionTable& getSectionTable()
  { return m_SectionTable; }

  const SectionTable& getSectionTable() const
  { return m_SectionTable; }

  sect_iterator sectBegin()
  { return m_SectionTable.begin(); }

  sect_iterator sectEnd()
  { return m_SectionTable.end(); }

  const_sect_iterator sectBegin() const
  { return m_SectionTable.begin(); }

  const_sect_iterator sectEnd() const
  { return m_SectionTable.end(); }

  LDSection* getSection(unsigned int pIdx);

  const LDSection* getSection(unsigned int pIdx) const;

  LDSection* getSection(const std::string& pName);

  const LDSection* getSection(const std::string& pName) const;

  size_t getSectionIdx(const std::string& pName) const;

  size_t numOfSections() const
  { return m_SectionTable.size(); }

  // -----  symbols  ----- //
  sym_iterator symBegin()
  { return m_SymTab.begin(); }

  sym_iterator symEnd()
  { return m_SymTab.end(); }

  const_sym_iterator symBegin() const
  { return m_SymTab.begin(); }

  const_sym_iterator symEnd() const
  { return m_SymTab.end(); }

  SymbolTable& symtab()
  { return m_SymTab; }

  const SymbolTable& symtab() const
  { return m_SymTab; }

private:
  SectionTable m_SectionTable;
  SymbolTable m_SymTab;

  // FIXME : maintain a map<section name, section index>
};


} // namespace of mcld

#endif

