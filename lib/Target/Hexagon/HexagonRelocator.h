//===-  HexagonRelocator.h ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef HEXAGON_RELOCATION_FACTORY_H
#define HEXAGON_RELOCATION_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/LD/Relocator.h>
#include <mcld/Target/GOT.h>
#include <mcld/Target/PLT.h>
#include <mcld/Target/SymbolEntryMap.h>
#include "HexagonLDBackend.h"

namespace mcld {

class ResolveInfo;

/** \class HexagonRelocator
 *  \brief HexagonRelocator creates and destroys the Hexagon relocations.
 *
 */
class HexagonRelocator : public Relocator
{
public:
  typedef SymbolEntryMap<PLTEntryBase> SymPLTMap;
  typedef SymbolEntryMap<HexagonGOTEntry> SymGOTMap;

public:
  HexagonRelocator(HexagonLDBackend& pParent);
  ~HexagonRelocator();

  Result applyRelocation(Relocation& pRelocation);

  HexagonLDBackend& getTarget()
  { return m_Target; }

  const HexagonLDBackend& getTarget() const
  { return m_Target; }

  const char* getName(Relocation::Type pType) const;

  Size getSize(Relocation::Type pType) const;

  const SymPLTMap& getSymPLTMap() const { return m_SymPLTMap; }
  SymPLTMap&       getSymPLTMap()       { return m_SymPLTMap; }

  const SymGOTMap& getSymGOTMap() const { return m_SymGOTMap; }
  SymGOTMap&       getSymGOTMap()       { return m_SymGOTMap; }

private:
  HexagonLDBackend& m_Target;
  SymPLTMap m_SymPLTMap;
  SymGOTMap m_SymGOTMap;
};

} // namespace of mcld

#endif

