/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCLDINPUTREE_H
#define MCLDINPUTREE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <iterator>
#include <stack>

namespace mcld
{
class MCLDFile;

/** \class MCLDInputTree
 *  \brief MCLDInputTree is the input tree to contains all inputs from the
 *  command line.
 *
 *  @see MCLDFile
 */
class MCLDInputTree
{
private:
  class Input
  {
    friend class MCLDInputTree;
    Input(MCLDFile* pFile);

  public:
    ~Input();

    Input* next()                   { return m_PositionalEdge; }
    Input* component()              { return m_InclusiveEdge; }

    bool isGroup() const            { return (0==m_pFile); }
    void precede(Input* pNext)      { m_PositionalEdge = pNext; }
    void contain(Input* pComponent) { m_InclusiveEdge = pComponent; }

  private:
    Input *m_PositionalEdge;
    Input *m_InclusiveEdge;
    MCLDFile *m_pFile;
  };

private:
  MCLDInputTree(const MCLDInputTree &); // DO NOT IMPLEMENT
  void operator=(const MCLDInputTree &); // DO NOT IMPLEMENT

public:
  MCLDInputTree();
  ~MCLDInputTree();

  // -----  iterator  -----
  friend class iterator;
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  // -----  observers -----
  unsigned int size() const;
  bool empty() const;

  // -----  modify  -----
  MCLDInputTree &append(MCLDFile* pFile);
  MCLDInputTree &enterGroup();
  MCLDInputTree &leaveGroup();

  MCLDInputTree &absorb(iterator pPosition, MCLDInputTree& pTree);
 
private:
  Input *m_pRoot;
  Input *m_pCurrent;
  unsigned int m_Size;
};

} // namespace of mcld

#endif

