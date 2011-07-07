/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef TARGETELFOBJECTREADER_H
#define TARGETELFOBJECTREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Target/TargetObjectReader.h>

namespace mcld
{

/** \class TargetELFObjectReader
 *  \brief TargetELFObjectReader provides an interface for target-dependent reader of ELF object files.
 *
 *  \see
 *  \author Luba Tang <lubatang@mediatek.com>
 */
class TargetELFObjectReader : public TargetObjectReader
{

};

} // namespace of mcld

#endif

