//===- RelocationFactory.h ------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
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
#include "mcld/Support/GCFactory.h"
#include "mcld/LD/Relocation.h"

namespace mcld
{

class LDSymbol;
class ResolveInfo;
class MCFragmentRef;

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
  RelocationFactory(size_t pNum);
  virtual ~RelocationFactory();

  virtual void destroy(Relocation* pRelocation) = 0;
 
  /// apply - general apply function
  virtual void apply(Relocation& pRelocation) = 0; 
  
  // ----- production ----- //
  Relocation* produce(Type pType,
                      MCFragmentRef& pFragRef,
		      DWord pTarget,
                      Address pAddend = 0);

  // TODO: RelocationFactory should hold the GOT/PLT offset
  // for applying functions
};

} // namespace of mcld

#endif

