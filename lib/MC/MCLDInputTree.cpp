/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#include <mcld/MC/MCLDInputTree.h>

using namespace mcld;

InputTree::Succeeder InputTree::Afterward;
InputTree::Includer  InputTree::Downward;

//===----------------------------------------------------------------------===//
// InputTree
InputTree::InputTree()
{
}

InputTree::~InputTree()
{
}

template<size_t DIRECT>
InputTree& InputTree::insert(InputTree::iterator pPosition,
                             InputTree::InputType pInputType,
                             const std::string& pNamespec,
                             const sys::fs::Path& pPath)
{
  BinaryTree<MCLDFile>::node_type* node = createNode();
  node->data = m_FileFactory.produce(pNamespec, pPath, pInputType); 
  if (pPosition.isRoot())
    proxy::hook<TreeIteratorBase::Leftward>(pPosition.m_pNode,
        const_cast<const node_type*>(node));
  else
    proxy::hook<DIRECT>(pPosition.m_pNode,
        const_cast<const node_type*>(node));
  return *this;
}

template<size_t DIRECT>
InputTree& InputTree::enterGroup(InputTree::iterator pPosition)
{
  NodeBase* node = createNode(); 
  if (pPosition.isRoot())
    proxy::hook<TreeIteratorBase::Leftward>(pPosition.m_pNode,
        const_cast<const node_type*>(node));
  else
    proxy::hook<DIRECT>(pPosition.m_pNode,
        const_cast<const node_type*>(node));
  return *this;
}


InputTree& InputTree::merge(InputTree::iterator pPosition, 
                            InputTree& pTree,
                            const InputTree::Connector& pConnector) 
{
  if (this == &pTree)
    return *this;

  if (!pTree.empty()) {
    pConnector.connect(pPosition, iterator(pTree.m_Root.node.right));
    BinaryTreeBase<MCLDFile>::m_Root.summon(
        pTree.BinaryTreeBase<MCLDFile>::m_Root);
    BinaryTreeBase<MCLDFile>::m_Root.delegate(pTree.m_Root);
    pTree.m_Root.node.left = pTree.m_Root.node.right = &pTree.m_Root.node;
  }
  return *this;
}


InputTree& InputTree::insert(InputTree::iterator pPosition,
                             InputTree::InputType pInputType,
                             const std::string& pNamespec,
                             const sys::fs::Path& pPath,
                             const InputTree::Connector& pConnector)
{
  BinaryTree<MCLDFile>::node_type* node = createNode();
  node->data = m_FileFactory.produce(pNamespec, pPath, pInputType);
  pConnector.connect(pPosition, iterator(node));
  return *this;
}


InputTree& InputTree::enterGroup(InputTree::iterator pPosition,
                                 const InputTree::Connector& pConnector)
{
  NodeBase* node = createNode();
  pConnector.connect(pPosition, iterator(node));
  return *this;
}

//===----------------------------------------------------------------------===//
// non-member functions
bool mcld::isGroup(const InputTree::iterator& pos)
{
  return !pos.hasData();
}

bool mcld::isGroup(const InputTree::const_iterator& pos)
{
  return !pos.hasData();
}

