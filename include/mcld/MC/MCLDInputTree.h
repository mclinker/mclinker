/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCLD_INPUTREE_H
#define MCLD_INPUTREE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <iterator>

namespace mcld
{
class MCLDFile;

/** \class InputTree
 *  \brief InputTree is the input tree to contains all inputs from the
 *  command line.
 *
 *  @see MCLDFile
 */
class InputTree
{
private:
  class Input
  {
    friend class InputTree;
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
  InputTree(const InputTree &); // DO NOT IMPLEMENT
  void operator=(const InputTree &); // DO NOT IMPLEMENT

public:
  InputTree();
  ~InputTree();

  // -----  iterator  -----
  friend class iterator;
//  iterator begin();
//  iterator end();
//  const_iterator begin() const;
//  const_iterator end() const;

  // -----  observers -----
//  unsigned int size() const;
//  bool empty() const;

  // -----  modify  -----
 
  unsigned int m_Size;
};

} // namespace of mcld

#endif

