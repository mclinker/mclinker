//===- LinkerConfig.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LinkerConfig.h>
#include <mcld/Config/Config.h>
#include <mcld/MC/InputFactory.h>
#include <mcld/MC/AttributeFactory.h>
#include <mcld/MC/ContextFactory.h>
#include <mcld/Support/FileSystem.h>
#include <string>

using namespace mcld;

//===----------------------------------------------------------------------===//
// LinkerConfig
//===----------------------------------------------------------------------===//
LinkerConfig::LinkerConfig(const std::string& pTripleString,
                           size_t pAttrNum,
                           size_t pInputNum)
  : m_Options(),
    m_Scripts(),
    m_Bitcode(),
    m_Triple(pTripleString)
{
  m_pAttrFactory = new AttributeFactory(pAttrNum);
  m_pCntxtFactory = new ContextFactory(pInputNum);
  m_pInputFactory = new InputFactory(pInputNum, *m_pAttrFactory);
  m_pInputTree = new InputTree(*m_pInputFactory);
  m_pOutput = new mcld::Output();
}

LinkerConfig::~LinkerConfig()
{
  delete m_pOutput;
  delete m_pAttrFactory;
  delete m_pCntxtFactory;
  delete m_pInputFactory;
  delete m_pInputTree;
}

void LinkerConfig::setBitcode(const sys::fs::Path& pPath, unsigned int pPosition)
{
  m_Bitcode.setPath(pPath);
  m_Bitcode.setPosition(pPosition);
}

const char* LinkerConfig::version()
{
  return MCLD_VERSION;
}
