/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef ARMELFARCHIVEREADER_H
#define ARMELFARCHIVEREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/MC/MCELFArchiveTargetReader.h>

namespace mcld
{

/** \class ARMELFArchiveReader
 *  \brief ARMELFArchiveReader is a target-dependent reader for ELF archive files.
 *
 *  \see
 *  \author Luba Tang <lubatang@mediatek.com>
 */
class ARMELFArchiveReader : public MCELFArchiveTargetReader
{
};


} // namespace of mcld

#endif

