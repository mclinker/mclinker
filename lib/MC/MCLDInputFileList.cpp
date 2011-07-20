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
  MCLDFile *NullFile = new MCLDFile();
  m_pEnd= new Node(*NullFile, UNKNOWN);
}

MCLDInputFileList::~MCLDInputFileList() {
  MCLDInputFileList::iterator it = begin() ;
  while (it != end()){
    iterator temp_it = it++;
    delete temp_it.m_pCurrent; 
  } 
  delete m_pEnd;
}

MCLDInputFileList &MCLDInputFileList::insert(MCLDFile *file, FileAttribute attr) {
  
  Node *newNodePtr = new Node(*file, attr);

  if(!m_pHead) { 
    m_pHead = newNodePtr;
  }
  else if (m_pTail->m_Attr == MCLDInputFileList::GROUP_START) {
    m_Stack.push(m_pTail);
    m_pTail->m_pChild = newNodePtr;  
  }
  else if (m_pTail->m_Attr == MCLDInputFileList::GROUP_END) { 
    m_pTail->m_pNext = 0;
    m_pTail = m_Stack.top();
    m_Stack.pop();
    m_pTail->m_pNext = newNodePtr;
  }
  else {
    m_pTail->m_pNext = newNodePtr; 
  }

  m_pTail = newNodePtr; 
  m_pTail->m_pNext = m_pEnd;
  return *this;
}

MCLDInputFileList &MCLDInputFileList::append(iterator position,
                                             iterator it_begin,
                                             iterator it_end) {
  position.m_pCurrent->m_Attr = LDSCRIPT; 
  if (!position.m_pCurrent->m_pChild) {
    position.m_pCurrent->m_pChild = it_begin.m_pCurrent;  
    it_end.m_pCurrent->m_pNext = 0;
  }
  return *this;
}

/// -------  MCLDInputFileList::Node  -------
MCLDInputFileList::Node::Node(MCLDFile &file,FileAttribute attr) 
  : m_pFile(&file), 
    m_pNext(0),
    m_pChild(0),
    visited(false),
    m_Attr(attr) {
}

MCLDInputFileList::Node::~Node() {
  delete m_pFile;
}

/// ------- iterator ---------
MCLDInputFileList::iterator &MCLDInputFileList::iterator::operator=(const MCLDInputFileList::iterator &it) {
  m_pCurrent = it.m_pCurrent;
  m_pFrom = it.m_pFrom;
  m_ITStack = it.m_ITStack;
  return *this;
}

MCLDInputFileList::iterator::iterator(MCLDInputFileList::Node *NP)
  : m_pCurrent(NP),
    m_pFrom(NP) {
}

MCLDInputFileList::iterator &MCLDInputFileList::iterator::operator++() {
/// FIXME
  return *this;
}

MCLDInputFileList::iterator MCLDInputFileList::iterator::operator++(int) {
  iterator tmp = *this;
  ++*this;
  return tmp;
}

MCLDFile &MCLDInputFileList::iterator::operator*() {
  return *(m_pCurrent->m_pFile);
}
