/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#include <mcld/MC/MCLDInputFileList.h>
#include <mcld/MC/MCLDFile.h>


using namespace mcld;

//==========================
// MCLDInputFileList

MCLDInputFileList::MCLDInputFileList () 
  : m_pHead(0),
    m_pTail(0) { 
}

MCLDInputFileList &MCLDInputFileList::insert(MCLDFile &file ,FileAttribute attr) {
  Node *newNodePtr = new Node(file,attr);

  if (attr == MCLDInputFileList::GROUP_START) 
    this->m_Stack.push(newNodePtr);
  else if (attr == MCLDInputFileList::GROUP_END) { 
    newNodePtr->m_pBack = this->m_Stack.top();
    this->m_Stack.pop();
  }

  if (m_pTail) {
    m_pTail->m_pNext = newNodePtr;
  }
  else {
    this->m_pHead = newNodePtr;
  } 
  this->m_pTail = newNodePtr;

  return *this;
}

MCLDInputFileList &MCLDInputFileList::insert(iterator position ,
                                             iterator it_begin ,
                                             iterator it_end) {
  it_end.m_pNode->m_pNext = position.m_pNode->m_pNext;
  position.m_pNode->m_pNext = it_begin.m_pNode;  
  return *this;
}

//MCLDInputFileList &
/// -------  MCLDInputFileList::Node  -------
MCLDInputFileList::Node::Node(MCLDFile &file,FileAttribute attr) 
  : m_File(file), 
    m_pNext(0),
    m_pBack(0),
    visited(false),
    m_Attr(attr) {
}


/// ------- iterator ---------
MCLDInputFileList::iterator &MCLDInputFileList::iterator::operator++() {
  if((m_pNode->m_pBack) && (!m_pNode->visited))
    m_pNode = m_pNode->m_pBack;
  else
    m_pNode = m_pNode->m_pNext;
  return *this;
}

MCLDInputFileList::iterator MCLDInputFileList::iterator::operator++(int) {
  iterator tmp = *this;
  ++*this;
  return tmp;
}

