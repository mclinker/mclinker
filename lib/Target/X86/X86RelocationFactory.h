/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef X86_RELOCATION_FACTORY_H
#define X86_RELOCATION_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/RelocationFactory.h>

namespace mcld
{

/** \class X86RelocationFactory
 *  \brief X86RelocationFactory creates and destroys X86Relocations.
 */
class X86RelocationFactory : public RelocationFactory
{
public:
  X86RelocationFactory(size_t pNum);
  ~X86RelocationFactory();

  /// getApply - get apply fnuction for relocation type pRelType
  const Pointer getApply(Type pRelType) const;

  void destroy(Relocation* pRelocation);

private:  
  /// m_ApplyFuncs - An array to map relocation type to its apply function
  static Pointer m_ApplyFuncs[];

};

} // namespace of mcld

#endif

