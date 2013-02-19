//===-  ARMRelocator.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ARM_RELOCATION_FACTORY_H
#define ARM_RELOCATION_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/LD/Relocator.h>
#include <mcld/Target/GOT.h>
#include <mcld/Target/SymbolEntryMap.h>
#include "ARMLDBackend.h"

namespace mcld {

/** \class ARMRelocator
 *  \brief ARMRelocator creates and destroys the ARM relocations.
 *
 */
class ARMRelocator : public Relocator
{
public:
  typedef SymbolEntryMap<ARMGOTEntry> SymGOTMap;
  typedef SymbolEntryMap<ARMPLT1> SymPLTMap;

public:
  ARMRelocator(ARMGNULDBackend& pParent);
  ~ARMRelocator();

  Result applyRelocation(Relocation& pRelocation);

  ARMGNULDBackend& getTarget()
  { return m_Target; }

  const ARMGNULDBackend& getTarget() const
  { return m_Target; }

  const char* getName(Relocation::Type pType) const;

  Size getSize(Relocation::Type pType) const;

  const SymGOTMap& getSymGOTMap() const { return m_SymGOTMap; }
  SymGOTMap&       getSymGOTMap()       { return m_SymGOTMap; }

  const SymPLTMap& getSymPLTMap() const { return m_SymPLTMap; }
  SymPLTMap&       getSymPLTMap()       { return m_SymPLTMap; }

  const SymGOTMap& getSymGOTPLTMap() const { return m_SymGOTPLTMap; }
  SymGOTMap&       getSymGOTPLTMap()       { return m_SymGOTPLTMap; }

private:
  ARMGNULDBackend& m_Target;
  SymGOTMap m_SymGOTMap;
  SymPLTMap m_SymPLTMap;
  SymGOTMap m_SymGOTPLTMap;
};

} // namespace of mcld

#endif

