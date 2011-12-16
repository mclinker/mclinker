//===- StrSymPool.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "mcld/LD/StrSymPool.h"
#include "mcld/LD/Resolver.h"
#include <llvm/Support/raw_ostream.h>

using namespace mcld;

//==========================
// StrSymPool
StrSymPool::StrSymPool(const Resolver& pResolver, StrSymPool::size_type pSize)
  : m_pResolver(pResolver.clone()), m_Table(pSize) {
}

StrSymPool::~StrSymPool()
{
  if (0 != m_pResolver)
    delete m_pResolver;
}

/// insertSymbol - insert a symbol and resolve it immediately
/// @return is the global symbol table changed
///
/// If the symbol is a local symbol, we don't care if there are existing symbols
/// with the same name. We insert the local symbol into the output's symbol table
/// immediately. On the other side, if the symbol is not a local symbol, we use
/// resolver to decide which symbol should be reserved.
///
bool StrSymPool::insertSymbol(const llvm::StringRef& pName,
                              bool pIsDyn,
                              ResolveInfo::Desc pDesc,
                              ResolveInfo::Binding pBinding,
                              ResolveInfo::ValueType pValue,
                              ResolveInfo::Visibility pVisibility)
{
  // insert local symbol immediately
  if (ResolveInfo::Local == pBinding) {
    return false;
  }

  // If the symbole is not a local symbol, we should check if there is any
  // symbol with the same name existed. If it already exists, we should use
  // resolver to decide which symbol should be reserved. Otherwise, we insert
  // the symbol and set up its attributes.
  bool exist = false;
  ResolveInfo* old_symbol = m_Table.insert(pName, exist);
  ResolveInfo* new_symbol = NULL;
  new_symbol = (exist && old_symbol->isSymbol())?
    m_Table.getEntryFactory().produce(pName):
    old_symbol;
  new_symbol->setSource(pIsDyn);
  new_symbol->setDesc(pDesc);
  new_symbol->setBinding(pBinding);
  new_symbol->setVisibility(pVisibility);
  new_symbol->setValue(pValue);
  if (!exist || !old_symbol->isSymbol())
    return true;

  // symbol resolution
  bool override = false;
  unsigned int action = Resolver::LastAction;
  switch(m_pResolver->resolve(*old_symbol, *new_symbol, override)) {
    case Resolver::Success:
      return override;
    case Resolver::Warning:
      llvm::errs() << "WARNING: " << m_pResolver->mesg() << "\n";
      m_pResolver->clearMesg();
      return override;
    case Resolver::Abort:
      llvm::report_fatal_error(m_pResolver->mesg());
      return false;
    default:
      return m_pResolver->resolveAgain(*this, action, *old_symbol, *new_symbol);
  }
}

llvm::StringRef StrSymPool::insertString(const llvm::StringRef& pString)
{
  bool exist = false;
  ResolveInfo* resolve_info = m_Table.insert(pString, exist);
  return llvm::StringRef(resolve_info->name(), resolve_info->nameSize());
}

void StrSymPool::reserve(StrSymPool::size_type pSize)
{
  m_Table.rehash(pSize);
}

StrSymPool::size_type StrSymPool::capacity() const
{
  return (m_Table.numOfBuckets() - m_Table.numOfEntries());
}

