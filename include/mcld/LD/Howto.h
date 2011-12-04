/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Diana Chen <diana.chen@mediatek.com>                                    *
 ****************************************************************************/
#ifndef LD_HOWTO_H
#define LD_HOWTO_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/Support/DataTypes.h>

namespace mcld
{
class Relocation;

/** \class Howto
 *  \brief Howto describes how to apply a relocation entry.
 */
class Howto
{
public:
  typedef uint64_t Address; // FIXME: use SizeTraits<T>::Address
  typedef uint64_t DWord; // FIXME: use SizeTraits<T>::DWord
  typedef uint8_t Type;
public:
   void apply(Relocation& pReloc);

   // -----  observers  ----- //
   Type type() const;
 
private:    
   Type m_Type;

};

} // namespace of mcld

#endif

