//===- MipsRelocator.h --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MIPS_RELOCATION_FACTORY_H
#define MIPS_RELOCATION_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/LD/Relocator.h>
#include <mcld/Support/GCFactory.h>
#include <mcld/Target/SymbolEntryMap.h>
#include "MipsLDBackend.h"

namespace mcld {

/** \class MipsRelocator
 *  \brief MipsRelocator creates and destroys the Mips relocations.
 */
class MipsRelocator : public Relocator
{
public:
  typedef SymbolEntryMap<MipsGOTEntry> SymGOTMap;

public:
  MipsRelocator(MipsGNULDBackend& pParent);

  Result applyRelocation(Relocation& pRelocation);

  MipsGNULDBackend& getTarget()
  { return m_Target; }

  const MipsGNULDBackend& getTarget() const
  { return m_Target; }

  // Get last calculated AHL.
  int32_t getAHL() const
  { return m_AHL; }

  // Set last calculated AHL.
  void setAHL(int32_t pAHL)
  { m_AHL = pAHL; }

  const char* getName(Relocation::Type pType) const;

  Size getSize(Relocation::Type pType) const;

  const SymGOTMap& getSymGOTMap() const { return m_SymGOTMap; }
  SymGOTMap&       getSymGOTMap()       { return m_SymGOTMap; }

private:
  MipsGNULDBackend& m_Target;
  int32_t m_AHL;
  SymGOTMap m_SymGOTMap;
};

} // namespace of mcld

#endif
