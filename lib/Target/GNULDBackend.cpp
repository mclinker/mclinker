//===- GNULDBackend.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "mcld/LD/ELFDSOWriter.h"
#include "mcld/LD/ELFEXEWriter.h"
#include "mcld/MC/MCGNUArchiveReader.h"
#include "mcld/MC/MCELFObjectReader.h"
#include "mcld/Target/GNULDBackend.h"


namespace mcld{

class MemoryArea;

//==========================
// GNULDBackend
GNULDBackend::GNULDBackend()
  : m_pObjectReader(0), m_pArchiveReader(0), m_pELFDSOWriter(0), m_pELFEXEWriter(0) {
}

GNULDBackend::~GNULDBackend()
{
  if (m_pObjectReader)
    delete m_pObjectReader;
  if (m_pArchiveReader)
    delete m_pArchiveReader;
  if (m_pELFDSOWriter)
    delete m_pELFDSOWriter;
  if (m_pELFEXEWriter)
    delete m_pELFEXEWriter;
}

MCObjectReader *GNULDBackend::getObjectReader()
{
  if (!m_pObjectReader)
    m_pObjectReader = new MCELFObjectReader(createObjectTargetReader());
  return m_pObjectReader;
}

MCArchiveReader *GNULDBackend::getArchiveReader()
{
  if (!m_pArchiveReader)
    m_pArchiveReader = new MCGNUArchiveReader();
  return m_pArchiveReader;
}

LDWriter *GNULDBackend::getDSOWriter(MemoryArea *Area, bool _IsLittleEndian)
{
  if (!m_pELFDSOWriter)
    m_pELFDSOWriter = new ELFDSOWriter(Area, _IsLittleEndian);
  return m_pELFDSOWriter;
}

LDWriter *GNULDBackend::getEXEWriter(MemoryArea *Area, bool _IsLittleEndian)
{
  if (!m_pELFEXEWriter)
    m_pELFEXEWriter = new ELFEXEWriter(Area, _IsLittleEndian);
  return m_pELFEXEWriter;
}

} //end namespace mcld
