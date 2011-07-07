/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCELFOBJECTTARGETWRITER_H
#define MCELFOBJECTTARGETWRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class MCELFObjectTargetWriter
 *  \brief MCELFObjectTargetWriter provides an abstract interface for all target ELF object writer.
 *
 *  \see
 *  \author Luba Tang <lubatang@mediatek.com>
 */
class MCELFObjectTargetWriter
{
protected:
  MCELFObjectTargetWriter();

public:
  virtual ~MCELFObjectTargetWriter();
};

} // namespace of mcld

#endif

