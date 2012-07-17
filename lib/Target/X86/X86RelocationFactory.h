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
#include "X86LDBackend.h"

namespace mcld
{

/** \class X86RelocationFactory
 *  \brief X86RelocationFactory creates and destroys the X86 relocations.
 *
 */
class X86RelocationFactory : public RelocationFactory
{
public:
  X86RelocationFactory(size_t pNum, X86GNULDBackend& pParent);
  ~X86RelocationFactory();

  Result applyRelocation(Relocation& pRelocation, const MCLDInfo& pLDInfo);

  X86GNULDBackend& getTarget()
  { return m_Target; }

  const X86GNULDBackend& getTarget() const
  { return m_Target; }

  const char* getName(Relocation::Type pType) const;

private:
  X86GNULDBackend& m_Target;
};

} // namespace of mcld

#endif

