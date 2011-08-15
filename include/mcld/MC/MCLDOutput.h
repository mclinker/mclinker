/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_OUTPUT_H
#define MCLD_OUTPUT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/MC/MCLDFile.h>

namespace mcld
{

/** \class MCLDOutput
 *  \brief MCLDOutput provides the information about the output.
 *
 *  \see
 *  \author Luba Tang <lubatang@mediatek.com>
 */
class Output : public MCLDFile
{

};

} // namespace of mcld

#endif

