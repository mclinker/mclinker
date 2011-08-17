/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   MCLDFile <pinronglu@gmail.com>                                          *
 ****************************************************************************/
#include <mcld/ADT/TypeTraits.h>
#include <BinTreeTest.h>
#include <iostream>
#include <string>

using namespace std;
using namespace mcld;
using namespace mcldtest;


// Constructor can do set-up work for all test here.
BinTreeTest::BinTreeTest()
{
	// create testee. modify it if need
	m_pTestee = new BinaryTree<int>();
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


/// General 
/*
TEST_F( BinTreeTest,Two_non_null_tree_merge) 
{
  BinaryTree<int>::iterator pos = m_pTestee->root();
  m_pTestee->join<TreeIteratorBase::Rightward>(pos,0);
  --pos;
  m_pTestee->join<TreeIteratorBase::Rightward>(pos,1);
  m_pTestee->join<TreeIteratorBase::Leftward>(pos,1);
  --pos;
  m_pTestee->join<TreeIteratorBase::Rightward>(pos,2);
  m_pTestee->join<TreeIteratorBase::Leftward>(pos,2);

  BinaryTree<int> *mergeTree = new BinaryTree<int>;
  BinaryTree<int>::iterator pos2 = mergeTree->root();
  mergeTree->join<TreeIteratorBase::Rightward>(pos2,1);
  --pos2;
  mergeTree->join<TreeIteratorBase::Rightward>(pos2,1);
  mergeTree->join<TreeIteratorBase::Leftward>(pos2,1);

  m_pTestee->merge<TreeIteratorBase::Rightward>(pos,*mergeTree); 
  delete mergeTree;
  EXPECT_TRUE(m_pTestee->size()==8);
}

/// ---- TEST - 2 ----
TEST_F( BinTreeTest, A_null_tree_merge_a_non_null_tree) 
{ 
  BinaryTree<int>::iterator pos = m_pTestee->root();
 
  BinaryTree<int> *mergeTree = new BinaryTree<int>;
  mergeTree->join<TreeIteratorBase::Rightward>(pos,0);
  --pos;
  mergeTree->join<TreeIteratorBase::Rightward>(pos,1);
  mergeTree->join<TreeIteratorBase::Leftward>(pos,1);
  --pos;
  mergeTree->join<TreeIteratorBase::Rightward>(pos,2);
  mergeTree->join<TreeIteratorBase::Leftward>(pos,2);

  m_pTestee->merge<TreeIteratorBase::Rightward>(pos,*mergeTree); 

  delete mergeTree;
  EXPECT_TRUE(m_pTestee->size()==5);
}

TEST_F( BinTreeTest, A_non_null_tree_merge_a_null_tree) 
{ 
  BinaryTree<int>::iterator pos = m_pTestee->root();
  m_pTestee->join<TreeIteratorBase::Rightward>(pos,0);
  --pos;
  m_pTestee->join<TreeIteratorBase::Rightward>(pos,1);
  m_pTestee->join<TreeIteratorBase::Leftward>(pos,1);
  --pos;
  m_pTestee->join<TreeIteratorBase::Rightward>(pos,2);
  m_pTestee->join<TreeIteratorBase::Leftward>(pos,2);
  
  BinaryTree<int> *mergeTree = new BinaryTree<int>;
  BinaryTree<int>::iterator pos2 = mergeTree->root(); 
  mergeTree->merge<TreeIteratorBase::Rightward>(pos2,*m_pTestee); 

  //delete m_pTestee;
  EXPECT_TRUE(mergeTree->size()==5);
}

TEST_F( BinTreeTest, Two_null_tree_merge) 
{ 
  BinaryTree<int>::iterator pos = m_pTestee->root();
 
  BinaryTree<int> *mergeTree = new BinaryTree<int>;
  BinaryTree<int>::iterator pos2 = mergeTree->root(); 

  mergeTree->merge<TreeIteratorBase::Rightward>(pos2,*m_pTestee); 

  //delete m_pTestee;
  EXPECT_TRUE(mergeTree->size()==0);
}

TEST_F( BinTreeTest, DFSIterator_BasicTraversal)
{
  int a = 111;
  BinaryTree<int>::iterator pos = m_pTestee->root();
  
  m_pTestee->join<TreeIteratorBase::Leftward>(pos,a);
  --pos;
  m_pTestee->join<TreeIteratorBase::Rightward>(pos,10);
  m_pTestee->join<TreeIteratorBase::Leftward>(pos,9);
  --pos;
  m_pTestee->join<TreeIteratorBase::Rightward>(pos,8);
  m_pTestee->join<TreeIteratorBase::Leftward>(pos,7);
  
  BinaryTree<int>::dfs_iterator dfs_it = m_pTestee->dfs_begin(); 
  BinaryTree<int>::dfs_iterator dfs_end = m_pTestee->dfs_end(); 

  for( ; dfs_it != dfs_end ; ++dfs_it ) 
  {
    cout<<**dfs_it<<"  ";
  } 
  cout<<endl; 
  BinaryTree<int>::bfs_iterator bfs_it = m_pTestee->bfs_begin(); 
  BinaryTree<int>::bfs_iterator bfs_end = m_pTestee->bfs_end(); 

  for( ; bfs_it != bfs_end ; ++bfs_it ) 
  {
    cout<<**bfs_it<<"  ";
  } 
  cout<<endl; 
}
*/
