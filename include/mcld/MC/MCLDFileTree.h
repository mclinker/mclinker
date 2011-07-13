/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCLDFILETREE_H
#define MCLDFILETREE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/MC/MCLDFile.h>
#include <stack>

namespace std {
class string;
}

namespace mcld {

class MCLDFileTree

{
public:
  MCLDFileTree();

  /// insert a MCLDFile at NEXT position of head
  void addMCLDFile(std::string, attribute = UNKNOW);
 
  /// encounter a MCLDFile with attribute GROUP_START ,push to stack
  /// go ahead until encounting GROUP_END ,pop and set m_pBack 
  void expand();		

  class iterator {
    public:
      iterator operator++(int);
      iterator &operator++();
      MCLDFile &operator*();
    private:
      MCLDFile *m_pCurrent;
  };
	friend class iterator;
  iterator begin();
  iterator end();
	
private:
  MCLDFile *m_pRoot;
  MCLDFile *m_pCurrent;
  std::stack<MCLDFile *> m_Stack;
};

} // namespace of mcld

#endif

