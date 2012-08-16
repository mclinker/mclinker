//===- InputBuilder.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/MC/InputBuilder.h>
#include <mcld/Support/Path.h>

using namespace mcld;

InputBuilder::InputBuilder(InputTree& pTree,
                           AttributeFactory& pAttributes)
  : m_InputTree(pTree), m_Attributes(pAttributes),
    m_pMove(&InputTree::Downward),
    m_Root(pTree.root()) {
}

InputTree& InputBuilder::enterGroup()
{
  m_InputTree.enterGroup(m_Root, *m_pMove);
  m_pMove->move(m_Root);
  m_ReturnStack.push(m_Root);
  m_pMove = &InputTree::Downward;

  return m_InputTree;
}

InputTree& InputBuilder::exitGroup()
{
  m_Root = m_ReturnStack.top();
  m_ReturnStack.pop();
  m_pMove = &InputTree::Afterward;

  return m_InputTree;
}

bool InputBuilder::isInGroup() const
{
  return !m_ReturnStack.empty();
}

