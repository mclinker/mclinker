/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCELFARCHIVETARGETREADER_H
#define MCELFARCHIVETARGETREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class MCELFArchiveTargetReader
 *  \brief MCELFArchiveTargetReader provides an interface for target-dependent archive reader.
 *
 *  \see
 *  \author Luba Tang <lubatang@mediatek.com>
 */
class MCELFArchiveTargetReader
{
protected:
  MCELFArchiveTargetReader();

public:
  virtual ~MCELFArchiveTargetReader();

};

} // namespace of mcld

#endif

