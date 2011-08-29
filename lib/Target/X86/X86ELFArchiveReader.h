/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef X86ELFARCHIVEREADER_H
#define X86ELFARCHIVEREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/MC/MCELFArchiveTargetReader.h>

namespace mcld
{

/** \class X86ELFArchiveReader
 *  \brief X86ELFArchiveReader is a target-dependent reader for ELF archive files.
 *
 *  \see
 *  \author Luba Tang <lubatang@mediatek.com>
 */
class X86ELFArchiveReader : public MCELFArchiveTargetReader
{
};


} // namespace of mcld

#endif

