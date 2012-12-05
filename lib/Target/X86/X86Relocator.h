//===-  X86Relocator.h --------------------------------------------===//
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

#include <mcld/LD/Relocator.h>
#include <mcld/Target/GOT.h>
#include <mcld/Target/PLT.h>
#include <mcld/Target/SymbolEntryMap.h>
#include "X86LDBackend.h"

namespace mcld {

class ResolveInfo;

/** \class X86Relocator
 *  \brief X86Relocator creates and destroys the X86 relocations.
 *
 */
class X86Relocator : public Relocator
{
public:
  typedef SymbolEntryMap<PLTEntryBase> SymPLTMap;
  typedef SymbolEntryMap<X86GOTEntry> SymGOTMap;
  typedef SymbolEntryMap<X86GOTPLTEntry> SymGOTPLTMap;

public:
  X86Relocator(X86GNULDBackend& pParent);
  ~X86Relocator();

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

  const SymGOTPLTMap& getSymGOTPLTMap() const { return m_SymGOTPLTMap; }
  SymGOTPLTMap&       getSymGOTPLTMap()       { return m_SymGOTPLTMap; }

private:
  X86GNULDBackend& m_Target;
  SymPLTMap m_SymPLTMap;
  SymGOTMap m_SymGOTMap;
  SymGOTPLTMap m_SymGOTPLTMap;
};

} // namespace of mcld

#endif

