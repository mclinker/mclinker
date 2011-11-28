/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/LD/ResolveInfo.h>

using namespace mcld;

//==========================
// ResolveInfo
ResolveInfo::ResolveInfo()
  : m_BitField(0) {
}

ResolveInfo::~ResolveInfo()
{
  m_BitField = 0x0;
}

void ResolveInfo::overrideAttributes(const ResolveInfo& pFrom)
{
}

void ResolveInfo::overrideVisibility(const ResolveInfo& pFrom)
{
}

void ResolveInfo::setDyn(bool pDyn)
{
}

void ResolveInfo::setType(ResolveInfo::Type pType)
{
}

void ResolveInfo::setBinding(ResolveInfo::Binding pBinding)
{
}

void ResolveInfo::setOther(uint8_t pOther)
{
}

void ResolveInfo::setVisibility(ResolveInfo::Visibility pVisibility)
{
}

bool ResolveInfo::isDyn() const
{
}

unsigned int ResolveInfo::type() const
{
}

unsigned int ResolveInfo::binding() const
{
}

uint8_t ResolveInfo::other() const
{
}

ResolveInfo::Visibility ResolveInfo::visibility() const
{
}

