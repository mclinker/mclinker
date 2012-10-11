//===- Relocation.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LD_RELOCATION_FACTORY_H
#define MCLD_LD_RELOCATION_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Support/GCFactory.h>
#include <mcld/Fragment/Relocation.h>

namespace mcld {

class LDSymbol;
class ResolveInfo;
class FragmentRef;
class FragmentLinker;
class Layout;
class GOT;
class TargetLDBackend;
class LinkerConfig;

/** \class RelocationFactory
 *  \brief RelocationFactory provides the interface for generating target
 *  relocation
 *
 */
class RelocationFactory : public GCFactory<Relocation, 0>
{
public:
  typedef Relocation::Type Type;
  typedef Relocation::Address Address;
  typedef Relocation::DWord DWord;
  typedef Relocation::SWord SWord;

  enum Result {
    OK,
    BadReloc,
    Overflow,
    Unsupport,
    Unknown
  };

public:
  explicit RelocationFactory(size_t pNum);

  virtual ~RelocationFactory();

  /// apply - general apply function
  virtual Result applyRelocation(Relocation& pRelocation) = 0;

  // ----- production ----- //
  /// produce - produce a relocation entry
  /// @param pType - the type of the relocation entry
  /// @param pFragRef - the place to apply the relocation
  /// @param pAddend - the addend of the relocation entry
  Relocation* produce(Type pType,
                      FragmentRef& pFragRef,
                      Address pAddend = 0);

  /// produceEmptyEntry - produce an empty relocation which
  /// occupied memory space but all contents set to zero.
  Relocation* produceEmptyEntry();

  void destroy(Relocation* pRelocation);

  void setFragmentLinker(const FragmentLinker& pLinker);

  // ------ observers -----//
  const FragmentLinker& getFragmentLinker() const;

  bool hasFragmentLinker() const;

  virtual TargetLDBackend& getTarget() = 0;

  virtual const TargetLDBackend& getTarget() const = 0;

  virtual const char* getName(Type pType) const = 0;

private:
  const FragmentLinker* m_pLinker;

};

} // namespace of mcld

#endif

