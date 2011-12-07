/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef ARM_RELOCATION_H
#define ARM_RELOCATION_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/Relocation.h>

namespace mcld
{

/** \class ARMRelocation
 *  \brief ARMRelocation is an abstract interface of all ARM-specific relocations.
 *
 */
class ARMRelocation : public Relocation
{
public:
  typedef Relocation::Type Type;
  typedef uint32_t Word; // FIXME: use SizeTrait<T>::Word instead
  typedef uint32_t Address; // FIXME: use SizeTrait<T>::Address instead
  typedef uint64_t DWord; // FIXME: use SizeTrait<T>::Word instead

public:
  ARMRelocation(Type pType,
                DWord& pTargetData,
                const MCFragmentRef& pTargetRef,
                Address pAddend,
                const ResolveInfo& pSymInfo);

  ~ARMRelocation();

  // TODO: list all common sub-functions here
  // Relocations should try to reuse these functions

  virtual void apply(Layout& pLayout) = 0;
};

} // namespace of mcld

#endif

