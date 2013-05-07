//===- ScriptInput.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/ScriptInput.h>
#include <mcld/Support/raw_ostream.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// ScriptInput
//===----------------------------------------------------------------------===//
ScriptInput::ScriptInput()
  : m_bAsNeeded(false)
{
}

ScriptInput::~ScriptInput()
{
}

void ScriptInput::append(const std::string& pPath)
{
  m_InputList.push_back(Node(pPath, m_bAsNeeded));
}

void ScriptInput::setAsNeeded(bool pEnable)
{
  m_bAsNeeded = pEnable;
}

