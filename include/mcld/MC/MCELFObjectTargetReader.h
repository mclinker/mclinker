/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCELFOBJECTTARGETREADER_H
#define MCELFOBJECTTARGETREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class MCELFObjectTargetReader
 *  \brief MCELFObjectTargetReader provides an abstract interface for target-dependent object readers.
 *
 *  \see
 *  \author Luba Tang <lubatang@mediatek.com>
 */
class MCELFObjectTargetReader
{
protected:
  MCELFObjectTargetReader();
public:
  virtual ~MCELFObjectTargetReader();

};

} // namespace of mcld

#endif

