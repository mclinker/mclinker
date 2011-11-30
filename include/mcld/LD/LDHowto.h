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
#include <mcld/LD/LDRelocation.h>

namespace mcld
{
class LDRelocation;

/** \class LDHowto
 *  \brief LDHowto describes how to apply a relocation entry.
 */
class LDHowto
{
public:
   void apply(LDRelocation& pRel);

   // -----  observers  ----- //
   uint32_t type() const;
 
private:    
   uint32_t m_Type;

};

} // namespace of mcld

#endif

