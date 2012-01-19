//===- MipsRelocationFactory.h --------------------------------------------===//
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

#include <mcld/LD/RelocationFactory.h>
#include <mcld/Support/GCFactory.h>
#include "MipsLDBackend.h"

namespace mcld
{

/** \class MipsRelocationFactory
 *  \brief MipsRelocationFactory creates and destroys the Mips relocations.
 */
class MipsRelocationFactory : public RelocationFactory
{
public:
  /** \enum Reloc
   *  \brief Reloc is the result of applying functions.
   */
  enum Result
  {
    OK,
    Overflow,
    BadReloc
  };

public:
  MipsRelocationFactory(size_t pNum, MipsGNULDBackend& pParent);

  void applyRelocation(Relocation& pRelocation, const MCLDInfo& pLDInfo);

  MipsGNULDBackend& getTarget()
  { return m_Target; }

  const MipsGNULDBackend& getTarget() const
  { return m_Target; }

private:
  MipsGNULDBackend& m_Target;
};

} // namespace of mcld

#endif
