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
#include <mcld/ADT/SizeTraits.h>
#include <mcld/ADT/TypeTraits.h>

namespace mcld
{

class Layout;

/// compunteFragmentSize - compute the specific MCFragment size
uint64_t computeFragmentSize(const Layout& pLayout,
                             const llvm::MCFragment& pFrag);

/** \class MCFragmentRef
 *  \brief MCFragmentRef is a reference of a MCFragment's contetnt.
 *
 */
class MCFragmentRef
{
public:
  typedef uint64_t Offset; // FIXME: use SizeTraits<T>::Offset
  typedef NonConstTraits<unsigned char>::pointer Address;
  typedef ConstTraits<unsigned char>::pointer ConstAddress;

public:
  MCFragmentRef();
  MCFragmentRef(llvm::MCFragment& pFrag, Offset pOffset = 0);
  ~MCFragmentRef();

  // -----  modifiers  ----- //
  MCFragmentRef& assign(const MCFragmentRef& pCopy);

  MCFragmentRef& assign(llvm::MCFragment& pFrag, Offset pOffset = 0);

  /// memcpy - copy memory
  /// copy memory from the fragment to the pDesc.
  /// @pDest - the destination address
  /// @pNBytes - copies pNBytes from the fragment[offset()+pOffset]
  /// @pOffset - additional offset. 
  ///            the start address offset from fragment[offset()]
  void memcpy(void* pDest, size_t pNBytes, Offset pOffset = 0) const;

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

