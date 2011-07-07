/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.lu@mediatek.com>                                          *
 ****************************************************************************/
#ifndef MCELFOBJECTWRITER_H
#define MCELFOBJECTWRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/MC/MCObjectWriter.h>

namespace mcld
{
class MCELFObjectTargetWriter;

/** \class MCELFObjectWriter
 *  \brief MCELFObjectWriter writes the target-independent parts of object files.
 *  MCELFObjectWriter reads a MCLDFile and writes into raw_ostream
 *
 *  \see MCLDFile raw_ostream
 *  \author Jush Lu <jush.lu@mediatek.com>
 */
class MCELFObjectWriter : public MCObjectWriter
{
public:
  MCELFObjectWriter(const MCELFObjectTargetWriter *pTargetWriter);
  ~MCELFObjectWriter();

private:
  const MCELFObjectTargetWriter *m_pTargetWriter;
};

} // namespace of mcld

#endif

