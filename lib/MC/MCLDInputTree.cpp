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
bool isGroup(const InputTree::iterator& pos)
{
  return !pos.hasData();
}

bool isGroup(const InputTree::const_iterator& pos)
{
  return !pos.hasData();
}

