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
    m_Triple(pTripleString),
    m_CodeGenType(Unknown)
{
  m_pAttrFactory = new AttributeFactory(pAttrNum);
  m_pInputFactory = new InputFactory(pInputNum, *m_pAttrFactory);
}

LinkerConfig::~LinkerConfig()
{
  delete m_pAttrFactory;
  delete m_pInputFactory;
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
