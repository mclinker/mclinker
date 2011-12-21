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
#include "X86LDBackend.h"

namespace mcld
{

/** \class X86RelocationFactory
 *  \brief X86RelocationFactory creates and destroys X86Relocations.
 */
class X86RelocationFactory : public RelocationFactory
{
public:
  X86RelocationFactory(size_t pNum, X86GNULDBackend& pParent);
  ~X86RelocationFactory();

  void applyRelocation(Relocation& pRelocation);

  X86GNULDBackend& getTarget()
  { return m_Target; }

  const X86GNULDBackend& getTarget() const
  { return m_Target; }

private:
  X86GNULDBackend& m_Target;
};

} // namespace of mcld

#endif

