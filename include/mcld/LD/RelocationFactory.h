//===- Relocation.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LD_RELOCATION_FACTORY_H
#define LD_RELOCATION_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Support/GCFactory.h>
#include <mcld/LD/Relocation.h>

namespace mcld
{

class LDSymbol;
class ResolveInfo;
class MCFragmentRef;
class Layout;
class GOT;
class TargetLDBackend;
class MCLDInfo;

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

public:
  explicit RelocationFactory(size_t pNum);

  virtual ~RelocationFactory();

  /// apply - general apply function
  virtual void applyRelocation(Relocation& pRelocation,
                               const MCLDInfo& pLDInfo) = 0;

  // ----- production ----- //
  /// produce - produce a relocation entry
  /// @param pType - the type of the relocation entry
  /// @param pFragRef - the place to apply the relocation
  /// @param pAddend - the addend of the relocation entry
  Relocation* produce(Type pType,
                      MCFragmentRef& pFragRef,
                      Address pAddend = 0);

  /// produceEmptyEntry - produce an empty relocation which
  /// occupied memory space but all contents set to zero.
  Relocation* produceEmptyEntry();

  void destroy(Relocation* pRelocation);

  void setLayout(const Layout& pLayout);

  // ------ observers -----//
  const Layout& getLayout() const;

  virtual TargetLDBackend& getTarget() = 0;

  virtual const TargetLDBackend& getTarget() const = 0;

private:
  const Layout* m_pLayout;

};

} // namespace of mcld

#endif

