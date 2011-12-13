//===- GNULDBackend.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "mcld/Target/GNULDBackend.h"

using namespace mcld;

//==========================
// GNULDBackend
GNULDBackend::GNULDBackend()
  : m_pArchiveReader(0),
    m_pObjectReader(0),
    m_pDynObjReader(0),
    m_pObjectWriter(0),
    m_pDynObjWriter(0) {
}

GNULDBackend::~GNULDBackend()
{
  if (m_pArchiveReader)
    delete m_pArchiveReader;
  if (m_pObjectReader)
    delete m_pObjectReader;
  if (m_pDynObjReader)
    delete m_pDynObjReader;
  if (m_pObjectWriter)
    delete m_pObjectWriter;
  if (m_pDynObjWriter)
    delete m_pDynObjWriter;
}

bool GNULDBackend::initArchiveReader(MCLinker&)
{
  // TODO
  return true;
}

bool GNULDBackend::initObjectReader(MCLinker&)
{
  // TODO
  return true;
}

bool GNULDBackend::initDynObjReader(MCLinker&)
{
  // TODO
  return true;
}

bool GNULDBackend::initObjectWriter(MCLinker&)
{
  // TODO
  return true;
}

bool GNULDBackend::initDynObjWriter(MCLinker&)
{
  // TODO
  return true;
}

ArchiveReader *GNULDBackend::getArchiveReader()
{
  // TODO
  return NULL;
}

ObjectReader *GNULDBackend::getObjectReader()
{
  // TODO
  return NULL;
}

DynObjReader *GNULDBackend::getDynObjReader()
{
  // TODO
  return NULL;
}

ObjectWriter *GNULDBackend::getObjectWriter()
{
  // TODO
  return NULL;
}

DynObjWriter *GNULDBackend::getDynObjWriter()
{
  // TODO
  return NULL;
}


