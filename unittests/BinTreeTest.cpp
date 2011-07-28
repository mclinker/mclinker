/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   MCLDFile <pinronglu@gmail.com>                                          *
 ****************************************************************************/
#include <mcld/Support/BinTree.h>
#include <BinTreeTest.h>
#include <iostream>
#include <strign>


using namespace std;
using namespace mcld;
using namespace mcldtest;


// Constructor can do set-up work for all test here.
BinTreeTest::BinTreeTest()
{
	// create testee. modify it if need
	m_pTestee = new BinTree<int>();
}

// Destructor can do clean-up work that doesn't throw exceptions here.
BinTreeTest::~BinTreeTest()
{
	delete m_pTestee;
}

// SetUp() will be called immediately before each test.
void BinTreeTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void BinTreeTest::TearDown()
{
}

//==========================================================================//
// Testcases
//

TEST_F( BinTreeTest, complete binTree,traversal) 
{
  BinTree::iterator pos = m_pTestee->root();
  /// complete binary tree
  for (int d=0 ; d<5 ; ++d) 
  { 
    m_pTestee->join<BinTree::iterator::Rightward>(pos,d);
    m_pTestee->join<BinTree::iterator::Leftward>(pos,d+1);
    ++pos;
  } 
  EXPECT_TRUE((*m_pTestee).size());
  EXPECT_TRUE(traversal(*m_pTestee));
  delete m_pTestee;
}

/// ---- TEST - 2 ----
TEST_F( BinTreeTest, Copy constructor, merge, empty) 
{
  BinTree::iterator pos = m_pTestee->root();
  /// Tree A
  m_pTestee->join<BinTree<int>::iterator::Rightward>(pos,1);
  m_pTestee->join<BinTree<int>::iterator::Leftward>(pos,1);
  ++pos;
  m_pTestee->join<BinTree<int>::iterator::Rightward>(pos,10);
  m_pTestee->join<BinTree<int>::iterator::Leftward>(pos,10);
  /// Tree B 
  BinTree<string> *mergeTree = new BinTree<string>();
  BinTree::iterator it = mergeTree->root();
  mergeTree->join<BinTree<string>::iterator::Rightward>(it,"lv.2");
  mergeTree->join<BinTree<string>::iterator::Rightward>(it,"lv.2");
  ++it; 
  /// merge
  (*mergeTree).merge(it,*m_pTestee);
  /// TEST_CASE
  EXPECT_TRUE(traversal(*mergeTree));
  EXPECT_TRUE((*m_pTestee).empty());
  EXPECT_TRUE((*mergeTree).size()==8);
  delete m_pMerge;  
}

template <class DataTy>
bool traversal(BinTree<DataTy> &tree) 
{
  BinTree::iterator it = tree.begin();
  BinTree::iterator end = tree.end();
  for ( ; it!=end ; ++it) {

  }
  return true;
}

