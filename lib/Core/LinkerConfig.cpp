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
#include <mcld/Support/FileSystem.h>
#include <string>

using namespace mcld;

//===----------------------------------------------------------------------===//
// LinkerConfig
//===----------------------------------------------------------------------===//
LinkerConfig::LinkerConfig(const std::string& pTripleString, size_t pInputNum)
  : m_Options(),
    m_Scripts(),
    m_Bitcode(),
    m_Attribute(),
    m_Triple(pTripleString),
    m_CodeGenType(Unknown)
{
  m_pInputFactory = new InputFactory(pInputNum, m_Attribute);
}

LinkerConfig::~LinkerConfig()
{
  delete m_pInputFactory;
}

const char* LinkerConfig::version()
{
  return MCLD_VERSION;
}
