//===- ScriptFile.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/LinkerScript/ScriptFile.h>
#include <mcld/LD/LinkerScript/ScriptCommand.h>
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/InputBuilder.h>
#include <mcld/InputTree.h>

using namespace mcld;

ScriptFile::ScriptFile(Input& pInput, InputBuilder& pBuilder)
  : m_Script(pInput),
    m_Builder(pBuilder)
{
  // FIXME: move creation of input tree out of Archive.
  m_pInputTree = new InputTree();
}

ScriptFile::~ScriptFile()
{
  for (iterator it = begin(), ie = end(); it != ie; ++it)
    delete *it;
  delete m_pInputTree;
}

void ScriptFile::dump() const
{
  for (const_iterator it = begin(), ie = end(); it != ie; ++it)
    (*it)->dump();
}

void ScriptFile::activate()
{
  for (const_iterator it = begin(), ie = end(); it != ie; ++it)
    (*it)->activate();
}

