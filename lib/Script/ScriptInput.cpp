//===- ScriptInput.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/ScriptInput.h>
#include <mcld/Script/StrToken.h>
#include <mcld/Support/raw_ostream.h>
#include <mcld/Support/GCFactory.h>
#include <llvm/Support/ManagedStatic.h>

using namespace mcld;

typedef GCFactory<ScriptInput, MCLD_SYMBOLS_PER_INPUT> ScriptInputFactory;
static llvm::ManagedStatic<ScriptInputFactory> g_ScriptInputFactory;

//===----------------------------------------------------------------------===//
// ScriptInput
//===----------------------------------------------------------------------===//
ScriptInput::ScriptInput()
{
}

ScriptInput::~ScriptInput()
{
}

void ScriptInput::push_back(StrToken* pToken)
{
  m_InputList.push_back(pToken);
}

ScriptInput* ScriptInput::create()
{
  ScriptInput* result = g_ScriptInputFactory->allocate();
  new (result) ScriptInput();
  return result;
}

void ScriptInput::destroy(ScriptInput*& pScriptInput)
{
  g_ScriptInputFactory->destroy(pScriptInput);
  g_ScriptInputFactory->deallocate(pScriptInput);
  pScriptInput = NULL;
}

void ScriptInput::clear()
{
  g_ScriptInputFactory->clear();
}
