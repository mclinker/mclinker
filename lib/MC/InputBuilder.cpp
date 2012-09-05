//===- InputBuilder.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/MC/InputBuilder.h>
#include <mcld/MC/ContextFactory.h>
#include <mcld/Support/Path.h>
#include <mcld/Support/MemoryAreaFactory.h>

using namespace mcld;

InputBuilder::InputBuilder(InputFactory& pInputFactory,
                           MemoryAreaFactory& pMemFactory,
                           ContextFactory& pContextFactory)
  : m_InputFactory(pInputFactory), m_MemFactory(pMemFactory),
    m_ContextFactory(pContextFactory), m_pCurrentTree(NULL), m_pMove(NULL),
    m_Root() {
}

InputTree* InputBuilder::createTree()
{
  m_pCurrentTree = new InputTree();
  m_Root = m_pCurrentTree->root();
  m_pMove = &InputTree::Downward;
  return m_pCurrentTree;
}

Input* InputBuilder::createInput(const std::string& pName,
                                 const sys::fs::Path& pPath,
                                 unsigned int pType,
                                 off_t pFileOffset)
{
  return m_InputFactory.produce(pName, pPath, pType, pFileOffset);
}

InputTree& InputBuilder::enterGroup()
{
  assert(NULL != m_pCurrentTree && NULL != m_pMove);

  m_pCurrentTree->enterGroup(m_Root, *m_pMove);
  m_pMove->move(m_Root);
  m_ReturnStack.push(m_Root);
  m_pMove = &InputTree::Downward;

  return *m_pCurrentTree;
}

InputTree& InputBuilder::exitGroup()
{
  assert(NULL != m_pCurrentTree && NULL != m_pMove);

  m_Root = m_ReturnStack.top();
  m_ReturnStack.pop();
  m_pMove = &InputTree::Afterward;

  return *m_pCurrentTree;
}

bool InputBuilder::isInGroup() const
{
  return !m_ReturnStack.empty();
}

const InputTree& InputBuilder::getCurrentTree() const
{
  assert(NULL != m_pCurrentTree && NULL != m_pMove);
  return *m_pCurrentTree;
}

InputTree& InputBuilder::getCurrentTree()
{
  assert(NULL != m_pCurrentTree && NULL != m_pMove);
  return *m_pCurrentTree;
}

void InputBuilder::setCurrentTree(InputTree& pInputTree)
{
  m_pCurrentTree = &pInputTree;
  m_Root = m_pCurrentTree->root();
  m_pMove = &InputTree::Downward;
}

bool InputBuilder::setContext(Input& pInput)
{
  LDContext* context = m_ContextFactory.produce(pInput.path());
  pInput.setContext(context);
  return true;
}

bool InputBuilder::setMemory(Input& pInput,
                             FileHandle::OpenMode pMode,
                             FileHandle::Permission pPerm)
{
  MemoryArea *memory = m_MemFactory.produce(pInput.path(), pMode, pPerm);

  if (!memory->handler()->isGood())
    return false;

  pInput.setMemArea(memory);
  return true;
}

