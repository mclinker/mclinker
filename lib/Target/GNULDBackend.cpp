//===- GNULDBackend.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Target/GNULDBackend.h>
#include <mcld/MC/MCLDOutput.h>
#include <llvm/Support/ELF.h>
#include <cassert>

using namespace mcld;

//==========================
// GNULDBackend
GNULDBackend::GNULDBackend()
  : m_pArchiveReader(0),
    m_pObjectReader(0),
    m_pDynObjReader(0),
    m_pObjectWriter(0),
    m_pDynObjWriter(0),
    m_pDynObjFileFormat(0),
    m_pExecFileFormat(0) {
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
  if (m_pDynObjFileFormat)
    delete m_pDynObjFileFormat;
  if (m_pExecFileFormat)
    delete m_pExecFileFormat;
}

size_t GNULDBackend::sectionStartOffset() const
{
  // FIXME: use fixed offset, we need 4 segments by default
  return sizeof(llvm::ELF::Elf64_Ehdr)+10*sizeof(llvm::ELF::Elf64_Phdr);
}

bool GNULDBackend::initArchiveReader(MCLinker&)
{
  if (0 == m_pArchiveReader)
    // FIXME: What is the correct ctor for GNUArchiveReader?
    m_pArchiveReader = new GNUArchiveReader();
  return true;
}

bool GNULDBackend::initObjectReader(MCLinker& pLinker)
{
  if (0 == m_pObjectReader)
    m_pObjectReader = new ELFObjectReader(*this, pLinker);
  return true;
}

bool GNULDBackend::initDynObjReader(MCLinker& pLinker)
{
  if (0 == m_pDynObjReader)
    m_pDynObjReader = new ELFDynObjReader(*this, pLinker);
  return true;
}

bool GNULDBackend::initObjectWriter(MCLinker&)
{
  // TODO
  return true;
}

bool GNULDBackend::initDynObjWriter(MCLinker& pLinker)
{
  if (0 == m_pDynObjWriter)
    m_pDynObjWriter = new ELFDynObjWriter(*this, pLinker);
  return true;
}

bool GNULDBackend::initExecSections(MCLinker& pMCLinker)
{
  if (0 == m_pExecFileFormat)
    m_pExecFileFormat = new ELFExecFileFormat();

  // initialize standard sections
  m_pExecFileFormat->initStdSections(pMCLinker);
  return true;
}

bool GNULDBackend::initDynObjSections(MCLinker& pMCLinker)
{
  if (0 == m_pDynObjFileFormat)
    m_pDynObjFileFormat = new ELFDynObjFileFormat();

  // initialize standard sections
  m_pDynObjFileFormat->initStdSections(pMCLinker);
  return true;
}

GNUArchiveReader *GNULDBackend::getArchiveReader()
{
  assert(0 != m_pArchiveReader);
  return m_pArchiveReader;
}

ELFObjectReader *GNULDBackend::getObjectReader()
{
  assert(0 != m_pObjectReader);
  return m_pObjectReader;
}

ELFDynObjReader *GNULDBackend::getDynObjReader()
{
  assert(0 != m_pDynObjReader);
  return m_pDynObjReader;
}

ELFObjectWriter *GNULDBackend::getObjectWriter()
{
  // TODO
  return NULL;
}

ELFDynObjWriter *GNULDBackend::getDynObjWriter()
{
  assert(0 != m_pDynObjWriter);
  return m_pDynObjWriter;
}


ELFDynObjFileFormat* GNULDBackend::getDynObjFileFormat()
{
  assert(0 != m_pDynObjFileFormat);
  return m_pDynObjFileFormat;
}

ELFExecFileFormat* GNULDBackend::getExecFileFormat()
{
  assert(0 != m_pExecFileFormat);
  return m_pExecFileFormat;
}

/// emitRegNamePools - emit regular name pools - .symtab, .strtab
uint64_t GNULDBackend::emitRegNamePools(Output& pOutput,
                                        const MCLDInfo& pLDInfo) const
{
  // TODO
  return 0;
}

/// emitNamePools - emit dynamic name pools - .dyntab, .dynstr, .hash
uint64_t GNULDBackend::emitDynNamePools(Output& pOutput,
                                        const MCLDInfo& pLDInfo) const
{
  // TODO
  return 0;
}

