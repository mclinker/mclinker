/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef LDSYMBOLTABLEIF_H
#define LDSYMBOLTABLEIF_H
#include <llvm/ADT/StringRef.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

class LDSymbol;
class LDSymbolTableStorage;


/** \class LDSymbolTableIF
 *  \brief Symbol table interface.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
class LDSymbolTableIF
{
  /* draft. */
  /* For the speed issue, iterator move to the next level interfase. It may be
   * come back after speed test.(Because the bound may be the "insertSymbol".)
   */
protected:
  LDSymbolTableIF(LDSymbolTableStorage *symtab):f_SymbolTableStrorage(symtab){}
  LDSymbolTableStorage *f_SymbolTableStrorage;
public:
  void insertSymbol(llvm::StringRef pSymName) { insertSymbol_impl(pSymName); }
  size_t size() const { return size_impl(); }
  void merge(const LDSymbolTableIF &pSymTab) { merge_impl(pSymTab); }
  virtual ~LDSymbolTableIF {}
public:
  /* Use NVI pattern let the iterator operation can be hidden by derived class.
   * Then the iterator operation can be fast when know the type.
   */
  typedef LDSymbolTableIterator<LDSymbol *> iterator;
  typedef LDSymbolTableIterator<LDSymbol *> const_iterator;
  const_iterator begin() const { return begin_impl(); }
  iterator begin() const { return begin_impl(); }
  const_iterator end() const { return end_impl(); }
  iterator end() const { return end_impl(); }
private:
  virtual void insertSymbol_impl(llvm::StringRef pSymName) {}
  virtual size_t size_impl() const {}
  virtual void merge_impl(const LDSymbolTableIF &pSymTab) {}
  virtual const_iterator begin() const { }
  virtual iterator begin() const { }
  virtual const_iterator end() const { }
  virtual iterator end() const { }
}

} // namespace of mcld

#endif

