//===- FileAction.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/MC/FileAction.h>
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/InputBuilder.h>
#include <mcld/MC/ContextFactory.h>
#include <mcld/Support/MemoryAreaFactory.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// ContextAction
//===----------------------------------------------------------------------===//
ContextAction::ContextAction(ContextFactory& pFactory,
                             unsigned int pPosition)
  : InputAction(pPosition), m_Factory(pFactory) {
}

bool ContextAction::activate(InputBuilder& pBuilder) const
{
  Input* input = *pBuilder.getCurrentNode();

  if (input->hasContext())
    return false;

  // already got type - for example, bitcode
  if (input->type() == Input::Script ||
      input->type() == Input::Object ||
      input->type() == Input::DynObj  ||
      input->type() == Input::Archive)
    return false;

  LDContext *context = m_Factory.produce(input->path());
  input->setContext(context);
  return true;
}

//===----------------------------------------------------------------------===//
// MemoryAreaAction 
//===----------------------------------------------------------------------===//
MemoryAreaAction::MemoryAreaAction(MemoryAreaFactory& pFactory,
                                   FileHandle::OpenMode pMode,
                                   FileHandle::Permission pPerm,
                                   unsigned int pPosition)
  : InputAction(pPosition),
    m_Factory(pFactory), m_Mode(pMode), m_Permission(pPerm) {
}

bool MemoryAreaAction::activate(InputBuilder& pBuilder) const
{
  Input* input = *pBuilder.getCurrentNode();

  if (input->hasMemArea())
    return false;

  // already got type - for example, bitcode
  if (input->type() == Input::Script ||
      input->type() == Input::Object ||
      input->type() == Input::DynObj  ||
      input->type() == Input::Archive)
    return false;

  MemoryArea *memory = m_Factory.produce(input->path(), m_Mode, m_Permission);

  if (!memory->handler()->isGood())
    return false;

  input->setMemArea(memory);
  return true;
}

