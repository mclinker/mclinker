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
#include <mcld/MC/InputFactory.h>
#include <mcld/MC/AttributeFactory.h>
#include <mcld/Support/FileHandle.h>

namespace mcld {

class MemoryAreaFactory;
class ContextFactory;

/** \class InputBuilder
 *  \brief InputBuilder recieves InputActions and build the InputTree.
 *
 *  InputBuilder build input tree and inputs.
 */
class InputBuilder
{
public:
  InputBuilder(InputFactory& pInputFactory,
               MemoryAreaFactory& pMemFactory,
               ContextFactory& pContextFactory);

  template<InputTree::Direction DIRECTION>
  InputTree& createNode(const std::string& pName,
                        const sys::fs::Path& pPath,
                        unsigned int pType = Input::Unknown);

  Input* createInput(const std::string& pName,
                     const sys::fs::Path& pPath,
                     unsigned int pType = Input::Unknown,
                     off_t pFileOffset = 0);

  bool setContext(Input& pInput);

  bool setMemory(Input& pInput,
                 FileHandle::OpenMode pMode,
		 FileHandle::Permission pPerm = FileHandle::System);

  InputTree& enterGroup();

  InputTree& exitGroup();

  bool isInGroup() const;

  // -----  accessors  ----- //
  const InputTree& getCurrentTree() const;
  InputTree&       getCurrentTree();

  /// createTree - create a new input tree and reset current node to the root
  /// of the new input tree.
  InputTree* createTree();

  void setCurrentTree(InputTree& pInputTree);

  const InputTree::iterator& getCurrentNode() const { return m_Root; }
  InputTree::iterator&       getCurrentNode()       { return m_Root; }

  const AttrConstraint& getConstraint() const { return m_InputFactory.constraint(); }
  AttrConstraint&       getConstraint()       { return m_InputFactory.constraint(); }

  const AttributeProxy& getAttributes() const { return m_InputFactory.last(); }
  AttributeProxy&       getAttributes()       { return m_InputFactory.last(); }

private:
  InputFactory& m_InputFactory;
  MemoryAreaFactory& m_MemFactory;
  ContextFactory& m_ContextFactory;

  InputTree* m_pCurrentTree;
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
  assert(NULL != m_pCurrentTree && NULL != m_pMove);

  Input* input = createInput(pName, pPath, pType);
  m_pCurrentTree->insert(m_Root, *m_pMove, *input);
  m_pMove->move(m_Root);
  m_pMove = &InputTree::Downward;

  return *m_pCurrentTree;
}

template<> inline InputTree&
InputBuilder::createNode<InputTree::Positional>(const std::string& pName,
                                               const sys::fs::Path& pPath,
                                               unsigned int pType)
{
  assert(NULL != m_pCurrentTree && NULL != m_pMove);

  Input* input = createInput(pName, pPath, pType);
  m_pCurrentTree->insert(m_Root, *m_pMove, *input);
  m_pMove->move(m_Root);
  m_pMove = &InputTree::Afterward;

  return *m_pCurrentTree;
}

} // end of namespace mcld

#endif

