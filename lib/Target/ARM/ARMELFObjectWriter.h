/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef ARMELFOBJECTWRITER_H
#define ARMELFOBJECTWRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/MC/MCELFObjectTargetWriter.h>

namespace mcld
{

/** \class ARMELFObjectWriter
 *  \brief ARMELFObjectWriter writes target-dependent parts of ELF object file.
 *
 *  \see
 *  \author Luba Tang <lubatang@mediatek.com>
 */
class ARMELFObjectWriter : public MCELFObjectTargetWriter
{

};

} // namespace of mcld

#endif

