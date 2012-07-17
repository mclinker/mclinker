//===-  ARMRelocationFactory.h --------------------------------------------===//
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

#include <mcld/LD/RelocationFactory.h>
#include <mcld/Target/GOT.h>
#include "ARMLDBackend.h"

namespace mcld
{

/** \class ARMRelocationFactory
 *  \brief ARMRelocationFactory creates and destroys the ARM relocations.
 *
 */
class ARMRelocationFactory : public RelocationFactory
{
public:
  ARMRelocationFactory(size_t pNum, ARMGNULDBackend& pParent);
  ~ARMRelocationFactory();

  Result applyRelocation(Relocation& pRelocation, const MCLDInfo& pLDInfo);

  ARMGNULDBackend& getTarget()
  { return m_Target; }

  const ARMGNULDBackend& getTarget() const
  { return m_Target; }

  const char* getName(Relocation::Type pType) const;

private:
  ARMGNULDBackend& m_Target;
};

} // namespace of mcld

#endif

