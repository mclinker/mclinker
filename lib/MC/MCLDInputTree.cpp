/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#include <mcld/MC/MCLDInputTree.h>

using namespace mcld;

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
}


InputTree& InputTree::enterGroup(InputTree::iterator pPosition,
                                 const InputTree::Connector& pConnector)
{
}


