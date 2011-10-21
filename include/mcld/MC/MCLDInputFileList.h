//===- MCLDInputFileList.h ------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLDINPUTFILELIST_H
#define MCLDINPUTFILELIST_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <stack>

namespace mcld
{
class MCLDFile;


class MCLDInputFileList
{
  MCLDInputFileList(const MCLDInputFileList &); // DO NOT IMPLEMENT
  void operator=(const MCLDInputFileList &); // DO NOT IMPLEMENT
public:
  MCLDInputFileList();
  ~MCLDInputFileList();

  enum FileAttribute {
    GROUP_START,
    GROUP_END,
    LDSCRIPT,
    UNKNOWN
  };

private:
  /// internal use only 
  class Node {
  public:
    Node(MCLDFile &, FileAttribute);
    ~Node();
    Node *m_pNext;
    Node *m_pChild;
    bool visited;

    FileAttribute m_Attr;
    MCLDFile *m_pFile;
  };

public:
  class iterator {
  friend class MCLDInputFileList;
  public:
    iterator(Node *);
    /// Assignment
    iterator &operator=(const iterator &RHS);

    /// Comparison operators
    bool operator==(const iterator &RHS) const { 
      return m_pCurrent == RHS.m_pCurrent;
    }
    bool operator!=(const iterator &RHS) const {
      return m_pCurrent != RHS.m_pCurrent;  
    }
   
    /// Increment and decrement operators
    iterator &operator++();  
    iterator operator++(int);

    /// Accessor
    MCLDFile &operator*();

  private:
    Node *m_pCurrent; 
    Node *m_pFrom;
    std::stack<Node *> m_ITStack;
  };

  friend class iterator;
  iterator begin() { return iterator(m_pHead); }
  iterator end() { return iterator(m_pTail); }
	
  MCLDInputFileList &insert(MCLDFile *, FileAttribute);
  MCLDInputFileList &append(iterator, iterator, iterator);
 
private:
  Node *m_pHead;
  Node *m_pTail;
  Node *m_pEnd;

  std::stack<Node *> m_Stack; 
};

} // namespace of mcld

#endif

