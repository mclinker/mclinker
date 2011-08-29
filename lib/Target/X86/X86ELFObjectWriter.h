/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef X86ELFOBJECTWRITER_H
#define X86ELFOBJECTWRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/MC/MCELFObjectTargetWriter.h>

namespace mcld
{

/** \class X86ELFObjectWriter
 *  \brief X86ELFObjectWriter writes target-dependent parts of ELF object file.
 *
 *  \see
 *  \author Luba Tang <lubatang@mediatek.com>
 */
class X86ELFObjectWriter : public MCELFObjectTargetWriter
{

};

} // namespace of mcld

#endif

