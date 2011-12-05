/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Diana Chen <diana.chen@mediatek.com>                                    *
 ****************************************************************************/
#include <mcld/LD/Howto.h>

using namespace mcld;

//==========================
// Howto
void Howto::apply(Relocation& pRel)
{
}


Howto::Type Howto::type() const
{
  return m_Type;
}


