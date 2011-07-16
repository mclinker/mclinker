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

/// -------  MCLDInputFileList::Node  -------
MCLDInputFileList::Node::Node(MCLDFile &file,FileAttribute attr) 
  : m_File(file), 
    m_pNext(0),
    m_pBack(0),
    m_Attr(attr) {
}


/// ------  MCLDInputFileList::Iterator  --------
MCLDInputFileList::iterator &MCLDInputFileList::iterator::insert(MCLDFile &file ,
                                                                MCLDInputFileList::FileAttribute attr) {	
  Node *nextPtr = new Node(file,attr);
  if(attr == MCLDInputFileList::GROUP_START) 
   // m_Stack.push_back(nextPtr);
  if(attr == MCLDInputFileList::GROUP_END)
   //nextPtr->m_pBack = m_Stack.pop_back();

  if( m_pNode ) { /// List is not empty
    /// Middel of list
    if(m_pNode->m_pNext) { 
      nextPtr->m_pNext = m_pNode->m_pNext;
      m_pNode->m_pNext = nextPtr;
    }
    m_pNode->m_pNext = nextPtr;
  }
  /// empty list
	else {
	}
}

MCLDFile *MCLDInputFileList::iterator::remove( MCLDInputFileList::iterator &IT ) {
}
