//===- FragmentRef.h ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LD_FRAGMENT_REFERENCE_H
#define MCLD_LD_FRAGMENT_REFERENCE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/ADT/SizeTraits.h>
#include <mcld/ADT/TypeTraits.h>
#include <mcld/LD/Fragment.h>

namespace mcld
{

class Layout;

/// compunteFragmentSize - compute the specific Fragment size
uint64_t computeFragmentSize(const Layout& pLayout,
                             const Fragment& pFrag);

/** \class FragmentRef
 *  \brief FragmentRef is a reference of a Fragment's contetnt.
 *
 */
class FragmentRef
{
public:
  typedef uint64_t Offset; // FIXME: use SizeTraits<T>::Offset
  typedef NonConstTraits<unsigned char>::pointer Address;
  typedef ConstTraits<unsigned char>::pointer ConstAddress;

public:
  FragmentRef();
  FragmentRef(Fragment& pFrag, Offset pOffset = 0);
  ~FragmentRef();

  // -----  modifiers  ----- //
  FragmentRef& assign(const FragmentRef& pCopy);

  FragmentRef& assign(Fragment& pFrag, Offset pOffset = 0);

  /// memcpy - copy memory
  /// copy memory from the fragment to the pDesc.
  /// @pDest - the destination address
  /// @pNBytes - copies pNBytes from the fragment[offset()+pOffset]
  /// @pOffset - additional offset. 
  ///            the start address offset from fragment[offset()]
  void memcpy(void* pDest, size_t pNBytes, Offset pOffset = 0) const;

  // -----  observers  ----- //
  Fragment* frag()
  { return m_pFragment; }

  const Fragment* frag() const
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
  Fragment* m_pFragment;
  Offset m_Offset;
};

} // namespace of mcld

#endif

