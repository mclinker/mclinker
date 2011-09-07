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
private:
  LDSymbolTableIF();
protected:
  LDSymbolTableStorage *f_SymbolTableStrorage;
public:
  virtual void insertSymbol(llvm::StringRef) {}
  virtual const_iterator begin() const {}
  virtual iterator begin() {}
  virtual const_iterator end() const {}
  virtual iterator end() {}
  virtual size_t size() {}
  virtual bool merge(const LDSymbolTableIF*) {}
  virtual ~LDSymbolTableIF {}
}

} // namespace of mcld

#endif

