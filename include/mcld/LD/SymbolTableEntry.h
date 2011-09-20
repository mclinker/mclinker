/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef SYMBOLTABLEENTRY_H
#define SYMBOLTABLEENTRY_H
#include <mcld/ADT/Uncopyable.h>
#include <mcld/LD/LDSymbol.h>
#include <vector>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace llvm
{
  class MCSectionData;
}

namespace mcld
{

/** \class SymbolTableEntry
 *  \brief 
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
template<typename ShouldOverwrite>
class SymbolTableEntry : private Uncopyable
{
  friend class SymbolStorage;
private:
  SymbolTableEntry(LDSymbol *pSym)
  : m_Symbol(pSym)
  {}

public:
  void addSameNameSymbol(const LDSymbol *sym) {
    ShouldOverwrite shouldOverwrite;
    if(shouldOverwrite(m_Symbol, sym)) {
      m_Sections.push_back(m_Symbol->section());
      m_Symbol->setDynamic(sym->isDyn());;
      m_Symbol->setType(sym->type());;
      m_Symbol->setBinding(sym->binding());
      m_Symbol->setSection(sym->section());
      m_Symbol->setValue(sym->value());
      m_Symbol->setSize(sym->size());
      m_Symbol->setOther(sym->other());
/*      const char *name = m_Symbol->name();
      *m_Symbol = sym;
      m_Symbol->setName(name);*/
    }
    else {
      m_Sections.push_back(sym->section());
    }
  }

  LDSymbol *symbol()
  { return m_Symbol; }

  const LDSymbol *symbol() const
  { return m_Symbol; }
private:
  LDSymbol *m_Symbol;
  std::vector<const llvm::MCSectionData *> m_Sections;
};

} // namespace of mcld

#endif

