/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCLDINPUTFILELIST_H
#define MCLDINPUTFILELIST_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <stack>
#include <string> ///FIXME : could be forward declaration

namespace mcld
{
class MCLDFile;


class MCLDInputFileList
{
public:
  MCLDInputFileList();
  ~MCLDInputFileList();

  enum FileAttribute {
    GROUP_START,
    GROUP_END,
    NORMAL_FILE,
    UNKNOW
  };

private:
  /// internal use only 
  class Node {
  public:
    Node(MCLDFile & ,FileAttribute);
    ~Node();
    Node *m_pNext;
    Node *m_pBack;

    FileAttribute m_Attr;
    MCLDFile &m_File;
  };

public:
  class iterator {
  friend class MCLDInputFileList;
  public:
    iterator( Node *NP ) : m_pNode(NP) {}
    iterator( Node &NR ) : m_pNode(&NR) {}

    /// Assignment
    iterator &operator=(const iterator &RHS) { 
      m_pNode = RHS.m_pNode; 
      return *this;
    }

    /// Comparison operators
    bool operator==(const iterator &RHS) const { 
      return m_pNode == RHS.m_pNode; 
    }
    bool operator!=(const iterator &RHS) const { 
      return m_pNode != RHS.m_pNode; 
    }
   
    /// Increment and decrement operators
    iterator &operator++() {
      m_pNode = m_pNode->m_pNext;
      return *this;
    }
    iterator operator++(int) {
      iterator tmp = *this;
      ++*this;
      return tmp;
    }    

    /// Accessor
    MCLDFile &operator*() {
      return m_pNode->m_File; 
    }

    iterator &insert(MCLDFile & ,FileAttribute);
    MCLDFile *remove(iterator &);

  private:
    Node *m_pNode;  
  };

  friend class iterator;
  iterator begin() { return iterator(m_pHead); }
  iterator end() { return iterator(m_pTail); }

  /// FIXME : users don't know what is Node
  void setHead(Node *head) { m_pHead = head; }
  void setTail(Node *tail) { m_pTail = tail; }

private:
  Node *m_pHead;
  Node *m_pTail;
 
  std::stack<Node *> m_Stack; 
};

} // namespace of mcld

#endif

