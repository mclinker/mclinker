/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef TARGETELFARCHIVEREADER_H
#define TARGETELFARCHIVEREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Target/TargetArchiveReader.h>

namespace mcld
{

/** \class TargetELFArchiveReader
 *  \brief TargetELFArchiveReader provides an interface for target-dependent reader of ELF archive files.
 *
 *  \see
 *  \author Luba Tang <lubatang@mediatek.com>
 */
class TargetELFArchiveReader : public TargetArchiveReader
{

};

} // namespace of mcld

#endif

