//===- GNULDBackend.cpp ---------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Target/GNULDBackend.h"
#include "mcld/MC/MCGNUArchiveReader.h"
#include "mcld/MC/MCELFObjectReader.h"
#include "mcld/MC/MCELFObjectWriter.h"

using namespace mcld;

//==========================
// GNULDBackend
GNULDBackend::GNULDBackend()
  : m_pObjectReader(0), m_pArchiveReader(0), m_pObjectWriter(0) {
}

GNULDBackend::~GNULDBackend()
{
  if (m_pObjectReader)
    delete m_pObjectReader;
  if (m_pArchiveReader)
    delete m_pArchiveReader;
  if (m_pObjectWriter)
    delete m_pObjectWriter;
}

mcld::MCObjectReader *GNULDBackend::getObjectReader()
{
  if (!m_pObjectReader)
    m_pObjectReader = new MCELFObjectReader(createObjectTargetReader());
  return m_pObjectReader;
}

mcld::MCArchiveReader *GNULDBackend::getArchiveReader()
{
  if (!m_pArchiveReader)
    m_pArchiveReader = new MCGNUArchiveReader();
  return m_pArchiveReader;
}

mcld::MCObjectWriter *GNULDBackend::getObjectWriter()
{
  if (!m_pObjectWriter)
    m_pObjectWriter = new MCELFObjectWriter(createObjectTargetWriter());
  return m_pObjectWriter;
}

