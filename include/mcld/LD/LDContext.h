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

  enum {
    RegSymTable, // Regular symbol table
    DynSymTable  // Dynamic symbol table
  };

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

  size_t numOfSections() const
  { return m_SectionTable.size(); }

  // -----  symbols  ----- //
  template<size_t CATEGORY>
  SymbolTable& getSymTable();

  template<size_t CATEGORY>
  const SymbolTable& getSymTable() const;

  template<size_t CATEGORY>
  sym_iterator symBegin();

  template<size_t CATEGORY>
  sym_iterator symEnd();

  template<size_t CATEGORY>
  const_sym_iterator symBegin() const;

  template<size_t CATEGORY>
  const_sym_iterator symEnd() const;

  SymbolTable& symtab()
  { return m_SymTab; }

  const SymbolTable& symtab() const
  { return m_SymTab; }

  SymbolTable& dynsym()
  { return m_DynSym; }

  const SymbolTable& dynsym() const
  { return m_DynSym; }

private:
  SectionTable m_SectionTable;
  SymbolTable m_SymTab;
  SymbolTable m_DynSym;
};

//===----------------------------------------------------------------------===//
// template proxy functions
namespace proxy
{
  template<size_t CATEGORY>
  inline LDContext::SymbolTable&
  get_symtab(LDContext* pContext) {
    assert(0 && "not allowed");
    static LDContext::SymbolTable fake_symtab;
    return fake_symtab;
  }

  template<size_t CATEGORY>
  inline const LDContext::SymbolTable&
  get_const_symtab(const LDContext* pContext) {
    assert(0 && "not allowed");
    return static_cast<const LDContext::SymbolTable&>(0);
  }

  template<>
  inline LDContext::SymbolTable&
  get_symtab<LDContext::RegSymTable>(LDContext* pContext) {
    return pContext->symtab();
  }

  template<>
  inline const LDContext::SymbolTable&
  get_const_symtab<LDContext::RegSymTable>(const LDContext* pContext) {
    return pContext->symtab();
  }

  template<>
  inline LDContext::SymbolTable&
  get_symtab<LDContext::DynSymTable>(LDContext* pContext) {
    return pContext->dynsym();
  }

  template<>
  inline const LDContext::SymbolTable&
  get_const_symtab<LDContext::DynSymTable>(const LDContext* pContext) {
    return pContext->dynsym();
  }

} // namespace of template proxy

//===----------------------------------------------------------------------===//
// template member functions
template<size_t CATEGORY>
LDContext::SymbolTable& LDContext::getSymTable()
{
  return proxy::get_symtab<CATEGORY>(this);
}

template<size_t CATEGORY>
const LDContext::SymbolTable& LDContext::getSymTable() const
{
  return proxy::get_const_symtab<CATEGORY>(this);
}

template<size_t CATEGORY>
LDContext::sym_iterator LDContext::symBegin()
{
  return proxy::get_symtab<CATEGORY>(this).begin();
}

template<size_t CATEGORY>
LDContext::sym_iterator LDContext::symEnd()
{
  return proxy::get_symtab<CATEGORY>(this).end();
}

template<size_t CATEGORY>
LDContext::const_sym_iterator LDContext::symBegin() const
{
  return proxy::get_const_symtab<CATEGORY>(this).begin();
}

template<size_t CATEGORY>
LDContext::const_sym_iterator LDContext::symEnd() const
{
  return proxy::get_const_symtab<CATEGORY>(this).end();
}

} // namespace of mcld

#endif

