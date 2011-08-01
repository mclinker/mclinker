/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef ARM_MCLDINFO_H
#define ARM_MCLDINFO_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/MC/MCLDInfo.h>

namespace mcld
{

class ARMELFLDInfo : public MCLDInfo
{
public:
  explicit ARMELFLDInfo();
};

} // namespace of mcld

#endif

