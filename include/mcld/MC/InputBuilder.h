//===- InputBuilder.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_MC_INPUT_BUILDER_H
#define MCLD_MC_INPUT_BUILDER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <string>
#include <stack>

#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/InputTree.h>
#include <mcld/MC/AttributeFactory.h>

namespace mcld {

/** \class InputBuilder
 *  \brief InputBuilder recieves InputActions and build the InputTree.
 */
class InputBuilder
{
public:
  InputBuilder(InputTree& pTree,
               AttributeFactory& pAttributes);

  template<InputTree::Direction DIRECTION>
  InputTree& createNode(const std::string& pName,
                        const sys::fs::Path& pPath,
                        unsigned int pType = Input::Unknown);

  InputTree& enterGroup();

  InputTree& exitGroup();

  bool isInGroup() const;

  // -----  accessors  ----- //
  InputTree& getTree()
  { return m_InputTree; }

  const InputTree& getTree() const
  { return m_InputTree; }

  InputTree::iterator& getCurrentNode()
  { return m_Root; }

  const InputTree::iterator& getCurrentNode() const
  { return m_Root; }

  AttrConstraint& getConstraint()
  { return m_Attributes.constraint(); }

  const AttrConstraint& getConstraint() const
  { return m_Attributes.constraint(); }

  AttributeProxy& getAttributes()
  { return m_Attributes.last(); }

  const AttributeProxy& getAttributes() const
  { return m_Attributes.last(); }

private:
  InputTree& m_InputTree;
  AttributeFactory& m_Attributes;

  InputTree::Mover* m_pMove;
  InputTree::iterator m_Root;
  std::stack<InputTree::iterator> m_ReturnStack;

};

//===----------------------------------------------------------------------===//
// Template implement
//===----------------------------------------------------------------------===//
template<> inline InputTree&
InputBuilder::createNode<InputTree::Inclusive>(const std::string& pName,
                                               const sys::fs::Path& pPath,
                                               unsigned int pType)
{
  m_InputTree.insert(m_Root, *m_pMove, pName, pPath, pType);
  m_pMove->move(m_Root);
  m_pMove = &InputTree::Downward;

  return m_InputTree;
}

template<> inline InputTree&
InputBuilder::createNode<InputTree::Positional>(const std::string& pName,
                                               const sys::fs::Path& pPath,
                                               unsigned int pType)
{
  m_InputTree.insert(m_Root, *m_pMove, pName, pPath, pType);
  m_pMove->move(m_Root);
  m_pMove = &InputTree::Afterward;

  return m_InputTree;
}

} // end of namespace mcld

#endif

