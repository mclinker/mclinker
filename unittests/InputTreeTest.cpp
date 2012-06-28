//===- InputTreeTest.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/MC/InputTree.h"
#include "mcld/MC/MCLDInfo.h"
#include <InputTreeTest.h>

using namespace mcld;
using namespace mcldtest;


// Constructor can do set-up work for all test here.
InputTreeTest::InputTreeTest()
{
	// create testee. modify it if need
	m_pAttr = new mcld::AttributeFactory(2);
        m_pAlloc = new mcld::InputFactory(10, *m_pAttr);
	m_pTestee = new InputTree(*m_pAlloc);
}

// Destructor can do clean-up work that doesn't throw exceptions here.
InputTreeTest::~InputTreeTest()
{
	delete m_pTestee;
	delete m_pAlloc;
	delete m_pAttr;
}

// SetUp() will be called immediately before each test.
void InputTreeTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void InputTreeTest::TearDown()
{
}

//==========================================================================//
// Testcases
//
TEST_F( InputTreeTest, Basic_operation ) {
  InputTree::iterator node = m_pTestee->root();
  m_pTestee->insert<InputTree::Inclusive>(node, "FileSpec", "path1");

  InputTree::const_iterator const_node = node;

  ASSERT_TRUE(isGroup(node));
  ASSERT_TRUE(isGroup(const_node));
  ASSERT_TRUE(m_pTestee->hasInput());
  ASSERT_EQ(1, m_pTestee->numOfInputs());

  --node;

  m_pTestee->enterGroup(node, InputTree::Downward);

  InputTree::const_iterator const_node2 = node;

  ASSERT_FALSE(node.isRoot());

  ASSERT_FALSE(isGroup(node));
  ASSERT_FALSE(isGroup(const_node2));
  ASSERT_TRUE(m_pTestee->hasInput());
  ASSERT_FALSE(m_pTestee->numOfInputs()==0);

  ASSERT_TRUE(m_pTestee->size()==2);
}

TEST_F( InputTreeTest, forLoop_TEST ) {
  InputTree::iterator node = m_pTestee->root();

  
  m_pTestee->insert<InputTree::Inclusive>(node, "FileSpec", "path1");
  InputTree::const_iterator const_node = node;
  --node;

  for(int i=0 ; i<100 ; ++i) 
  {
    m_pTestee->insert<InputTree::Inclusive>(node,"FileSpec", "path1");
    ++node;
  }

  m_pTestee->enterGroup(node, InputTree::Downward);
  --node;

  ASSERT_FALSE(node.isRoot());
  ASSERT_TRUE(isGroup(node));
  ASSERT_TRUE(m_pTestee->hasInput());
  ASSERT_FALSE(m_pTestee->numOfInputs()==100);

  ASSERT_TRUE(m_pTestee->size()==102);
}

TEST_F( InputTreeTest, Nesting_Case ) {
  InputTree::iterator node = m_pTestee->root(); 

  for(int i=0 ; i<50 ; ++i) 
  {
    m_pTestee->enterGroup(node, InputTree::Downward);
    --node;

    m_pTestee->insert(node, InputTree::Afterward, "FileSpec", "path1");
    ++node;
  }
  
  ASSERT_FALSE(node.isRoot());
  ASSERT_FALSE(isGroup(node));
  ASSERT_TRUE(m_pTestee->hasInput());
  ASSERT_TRUE(m_pTestee->numOfInputs()==50);
  ASSERT_TRUE(m_pTestee->size()==100);
}

TEST_F( InputTreeTest, DFSIterator_BasicTraversal)
{
  
  InputTree::iterator node = m_pTestee->root(); 
  m_pTestee->insert<InputTree::Inclusive>(node, "111", "/");
  node.move<InputTree::Inclusive>();

  m_pTestee->insert<InputTree::Positional>(node, "10", "/");
  m_pTestee->enterGroup<InputTree::Inclusive>(node);
  node.move<InputTree::Inclusive>();
  m_pTestee->insert<InputTree::Inclusive>(node, "7", "/");
  m_pTestee->insert<InputTree::Positional>(node, "8", "/");

  InputTree::dfs_iterator dfs_it = m_pTestee->dfs_begin(); 
  InputTree::dfs_iterator dfs_end = m_pTestee->dfs_end(); 
  ASSERT_STREQ("111", (*dfs_it)->name().c_str());
  ++dfs_it;
  ASSERT_STREQ("7", (**dfs_it).name().c_str());
  ++dfs_it;
  ASSERT_STREQ("8", (**dfs_it).name().c_str());
  ++dfs_it;
  ASSERT_STREQ("10", (**dfs_it).name().c_str());
  ++dfs_it;
  ASSERT_TRUE(dfs_it ==  dfs_end);
}

