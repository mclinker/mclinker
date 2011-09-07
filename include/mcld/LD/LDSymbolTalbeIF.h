/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef LDSYMBOLTALBEIF_H
#define LDSYMBOLTALBEIF_H
#include <llvm/ADT/StringRef.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class LDSymbolTalbeIF
 *  \brief Symbol table interface.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
class LDSymbol;
class LDSymbolTableStorage;

class LDSymbolTalbeIF
{
  /* draft. */
private:
  LDSymbolTalbeIF();
protected:
  LDSymbolTableStorage *f_SymbolTableStrorage;
public:
  virtual void insertSymbol(llvm::StringRef) {}
  virtual const_iterator begin() const {}
  virtual iterator begin() {}
  virtual const_iterator end() const {}
  virtual iterator end() {}
  virtual size_t size() {}
  virtual bool merge(const LDSymbolTalbeIF*) {}
  virtual ~LDSymbolTalbeIF {}
}

} // namespace of mcld

#endif

