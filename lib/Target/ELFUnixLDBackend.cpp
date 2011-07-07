/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Target/ELFUnixLDBackend.h>
#include <mcld/MC/MCELFArchiveReader.h>
#include <mcld/MC/MCELFObjectReader.h>
#include <mcld/MC/MCELFObjectWriter.h>

using namespace mcld;


//==========================
// ELFUnixLDBackend
ELFUnixLDBackend::ELFUnixLDBackend()
  : m_pObjectReader(0), m_pArchiveReader(0), m_pObjectWriter(0) {
}

ELFUnixLDBackend::~ELFUnixLDBackend()
{
  if (m_pObjectReader)
    delete m_pObjectReader;
  if (m_pArchiveReader)
    delete m_pArchiveReader;
  if (m_pObjectWriter)
    delete m_pObjectWriter;
}

MCObjectReader *ELFUnixLDBackend::getObjectReader()
{
  if (!m_pObjectReader)
    m_pObjectReader = new MCELFObjectReader(createObjectTargetReader());
  return m_pObjectReader;
}

MCArchiveReader *ELFUnixLDBackend::getArchiveReader()
{
  if (!m_pArchiveReader)
    m_pArchiveReader = new MCELFArchiveReader(createArchiveTargetReader());
  return m_pArchiveReader;
}

MCObjectWriter *ELFUnixLDBackend::getObjectWriter()
{
  if (!m_pObjectWriter)
    m_pObjectWriter = new MCELFObjectWriter(createObjectTargetWriter());
  return m_pObjectWriter;
}

