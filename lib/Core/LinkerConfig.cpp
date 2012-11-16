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

#include <mcld/Support/MsgHandling.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// LinkerConfig
//===----------------------------------------------------------------------===//
LinkerConfig::LinkerConfig()
  : m_Options(),
    m_Scripts(),
    m_Bitcode(),
    m_Attribute(),
    m_Triple(),
    m_CodeGenType(Unknown)
{
  // FIXME: is here the right place to hold this?
  InitializeDiagnosticEngine(*this);
}

LinkerConfig::LinkerConfig(const std::string& pTripleString)
  : m_Options(),
    m_Scripts(),
    m_Bitcode(),
    m_Attribute(),
    m_Triple(pTripleString),
    m_CodeGenType(Unknown)
{
  // FIXME: is here the right place to hold this?
  InitializeDiagnosticEngine(*this);
}

LinkerConfig::~LinkerConfig()
{
  // FIXME: is here the right place to hold this?
  FinalizeDiagnosticEngine();
}

void LinkerConfig::setTriple(const llvm::Triple& pTriple)
{
  m_Triple = pTriple;
}

void LinkerConfig::setTriple(const std::string& pTriple)
{
  m_Triple.setTriple(pTriple);
}

const char* LinkerConfig::version()
{
  return MCLD_VERSION;
}
