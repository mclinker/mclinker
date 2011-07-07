/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/MC/MCELFObjectWriter.h>

using namespace mcld;

//==========================
// MCELFObjectWriter
MCELFObjectWriter::MCELFObjectWriter(const MCELFObjectTargetWriter *pTargetWriter)
  : m_pTargetWriter(pTargetWriter) {
}

MCELFObjectWriter::~MCELFObjectWriter()
{
}


