/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#include <mcld/MC/MCLDFileTree.h>



using namespace mcld;

//==========================
// MCLDFileTree

MCLDFileTree::MCLDFileTree()
  : m_pRoot(0),
    m_pCurrent(0) {
}

MCLDFileTree::~MCLDFileTree() {
   /// traversal the tree and delete it
}

void MCLDFileTree::addMCLDFile(std::string str, attribute attr = UNKNOW) {
  if(m_pRoot) {
    m_pCurrent->setNext( new MCLDFile(str ,attr) ); 
    m_pCurrent = m_pCurrent->getNext(); 
    assert( m_pCurrent->getNext() != NULL );
  }
  else { 
    m_pRoot = new MCLDFile(str ,attr);
    m_pCurrent = m_pRoot;
  }
}

