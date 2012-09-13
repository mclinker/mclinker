//===-  X86RelocationFactory.h --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef X86_RELOCATION_FACTORY_H
#define X86_RELOCATION_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/LD/RelocationFactory.h>
#include <mcld/Target/GOT.h>
#include <mcld/Target/PLT.h>
#include <mcld/Target/SymbolEntryMap.h>
#include "X86LDBackend.h"

namespace mcld {

class ResolveInfo;

/** \class X86RelocationFactory
 *  \brief X86RelocationFactory creates and destroys the X86 relocations.
 *
 */
class X86RelocationFactory : public RelocationFactory
{
public:
  typedef SymbolEntryMap<PLT::Entry> SymPLTMap;
  typedef SymbolEntryMap<GOT::Entry> SymGOTMap;

public:
  X86RelocationFactory(size_t pNum, X86GNULDBackend& pParent);
  ~X86RelocationFactory();

  Result applyRelocation(Relocation& pRelocation);

  X86GNULDBackend& getTarget()
  { return m_Target; }

  const X86GNULDBackend& getTarget() const
  { return m_Target; }

  const char* getName(Relocation::Type pType) const;

  const SymPLTMap& getSymPLTMap() const { return m_SymPLTMap; }
  SymPLTMap&       getSymPLTMap()       { return m_SymPLTMap; }

  const SymGOTMap& getSymGOTMap() const { return m_SymGOTMap; }
  SymGOTMap&       getSymGOTMap()       { return m_SymGOTMap; }

  const SymGOTMap& getSymGOTPLTMap() const { return m_SymGOTPLTMap; }
  SymGOTMap&       getSymGOTPLTMap()       { return m_SymGOTPLTMap; }

private:
  X86GNULDBackend& m_Target;
  SymPLTMap m_SymPLTMap;
  SymGOTMap m_SymGOTMap;
  SymGOTMap m_SymGOTPLTMap;
};

} // namespace of mcld

#endif

