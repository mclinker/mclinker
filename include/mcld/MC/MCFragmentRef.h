//===- MCFragmentRef.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_MCFRAGMENT_REFERENCE_H
#define MCLD_MCFRAGMENT_REFERENCE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/MC/MCAssembler.h>
#include "mcld/ADT/SizeTraits.h"
#include "mcld/ADT/TypeTraits.h"

namespace mcld
{

/** \class MCFragmentRef
 *  \brief MCFragmentRef is a reference of a MCFragment's contetnt.
 *
 */
class MCFragmentRef
{
public:
  typedef size_t Offset; // FIXME: use SizeTraits<T>::Offset
  typedef NonConstTraits<char>::pointer Address;
  typedef ConstTraits<char>::pointer ConstAddress;

public:
  MCFragmentRef();
  MCFragmentRef(llvm::MCFragment& pFrag, Offset pOffset = 0);
  ~MCFragmentRef();

  // -----  modifiers  ----- //
  void assign(llvm::MCFragment& pFrag, Offset pOffset = 0);

  // -----  observers  ----- //
  llvm::MCFragment* frag()
  { return m_pFragment; }

  const llvm::MCFragment* frag() const
  { return m_pFragment; }

  Offset offset() const
  { return m_Offset; }

  // -----  dereference  ----- //
  Address deref();

  ConstAddress deref() const;

  Address operator*()
  { return deref(); }

  ConstAddress operator*() const
  { return deref(); }
  
private:
  llvm::MCFragment* m_pFragment;
  Offset m_Offset;
};

} // namespace of mcld

#endif

