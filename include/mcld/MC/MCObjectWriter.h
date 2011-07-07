/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCOBJECTWRITER_H
#define MCOBJECTWRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class MCObjectWriter
 *  \brief MCObjectWriter provides a common interface for object file writers.
 *
 *  \see
 *  \author Luba Tang <lubatang@mediatek.com>
 */
class MCObjectWriter
{
protected:
  MCObjectWriter();

public:
  virtual ~MCObjectWriter();
};

} // namespace of mcld

#endif

