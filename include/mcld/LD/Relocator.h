//===- Relocator.h --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_RELOCATOR_H
#define MCLD_RELOCATOR_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Fragment/Relocation.h>

namespace mcld
{

class FragmentLinker;
class TargetLDBackend;

/** \class Relocator
 *  \brief Relocator provides the interface of performing relocations
 */
class Relocator
{
public:
  typedef Relocation::Type Type;
  typedef Relocation::Address Address;
  typedef Relocation::DWord DWord;
  typedef Relocation::SWord SWord;

public:
  enum Result {
    OK,
    BadReloc,
    Overflow,
    Unsupport,
    Unknown
  };

public:
  virtual ~Relocator() {}

  /// apply - general apply function
  virtual Result applyRelocation(Relocation& pRelocation) = 0;

  void setFragmentLinker(const FragmentLinker& pLinker)
  { m_pLinker = &pLinker; }

  // ------ observers -----//
  const FragmentLinker& getFragmentLinker() const
  {
    assert(NULL != m_pLinker);
    return *m_pLinker;
  }

  bool hasFragmentLinker() const
  { return (NULL != m_pLinker); }

  virtual TargetLDBackend& getTarget() = 0;

  virtual const TargetLDBackend& getTarget() const = 0;

  virtual const char* getName(Type pType) const = 0;

private:
  const FragmentLinker* m_pLinker;

};

} // namespace of mcld

#endif

