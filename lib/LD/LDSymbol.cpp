/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.lu@mediatek.com> (owner)                                  *
 *   Nowar Gu <nowar100@gmail.com>  (early prototype)                        *
 *   Luba Tang <luba.tang@mediatek.com> (consistent prototype with proposal) *
 ****************************************************************************/
#include <mcld/LD/LDSymbol.h>

using namespace mcld;

LDSymbol::LDSymbol()
 : m_pName(0), m_IsDyn(true), m_Type(NoneType), m_Binding(NoneBinding),
   m_pSection(0), m_Value(0), m_Size(0), m_Other(0) {
}

LDSymbol::~LDSymbol()
{
}

