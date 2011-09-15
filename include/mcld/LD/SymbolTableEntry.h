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
  SymbolTableEntry();

public:
  void addSameNameSymbol(const LDSymbol &sym) {
    if(ShouldOverwrite(m_Symbol, sym)) {
      m_Sections.push_back(m_Symbol.section());
      m_Symbol = sym;
    }
    else {
      m_Sections.push_back(sym.section());
    }
  }
  LDSymbol &getSymbol(){ return m_Symbol; }

private:
  LDSymbol m_Symbol;
  vector<llvm::MCSectionData *> m_Sections;
};

} // namespace of mcld

#endif

