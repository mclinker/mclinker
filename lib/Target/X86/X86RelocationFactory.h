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
#include "mcld/LD/RelocationFactory.h"

namespace mcld
{

/** \class X86RelocationFactory
 *  \brief X86RelocationFactory creates and destroys X86Relocations.
 */
class X86RelocationFactory : public RelocationFactory
{
public:
  typedef void (X86RelocationFactory::*Pointer)(Relocation&); 

public:
  X86RelocationFactory(size_t pNum);
  ~X86RelocationFactory();

  void apply(Relocation& pRelocation);

private:  
  /// m_ApplyFuncs - An array to map relocation type to its apply function
  static Pointer m_ApplyFuncs[];

};

} // namespace of mcld

#endif

