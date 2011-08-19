/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include "PathTest.h"
//
#include <iostream>
#include <string>
using namespace std;
//
using namespace mcld;
using namespace mcld::sys::fs;
using namespace mcldtest;


// Constructor can do set-up work for all test here.
PathTest::PathTest()
{
	// create testee. modify it if need
	m_pTestee = new Path();
}

// Destructor can do clean-up work that doesn't throw exceptions here.
PathTest::~PathTest()
{
	delete m_pTestee;
}

// SetUp() will be called immediately before each test.
void PathTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void PathTest::TearDown()
{
}

//==========================================================================//
// Testcases
//




TEST_F( PathTest, should_exist ) {
  const std::string root = "Makefile";
  m_pTestee->assign(root);
  EXPECT_TRUE(exists(*m_pTestee));
  
  delete m_pTestee;
  m_pTestee = new Path(root);
  EXPECT_TRUE(exists(*m_pTestee));
}

TEST_F( PathTest, should_not_exist ) {
  const std::string root = "/fuck";
  m_pTestee->assign(root);
  EXPECT_FALSE(exists(*m_pTestee));
  
  delete m_pTestee;
  m_pTestee = new Path(root);
  EXPECT_FALSE(exists(*m_pTestee));
}

TEST_F( PathTest, should_is_directory ) {
//  const std::string root = "/proj/mtk03931/temp/pndk-luba/../";
  const std::string root = "../././..";
  m_pTestee->assign(root);
  EXPECT_TRUE(exists(*m_pTestee));
  EXPECT_TRUE(is_directory(*m_pTestee));
  delete m_pTestee;
  m_pTestee = new Path(root);
  EXPECT_TRUE(exists(*m_pTestee));
  EXPECT_TRUE(is_directory(*m_pTestee));
}

TEST_F( PathTest, should_not_is_directory ) {
  const std::string root = "/fuck";
  m_pTestee->assign(root);
  EXPECT_FALSE(exists(*m_pTestee));
  EXPECT_FALSE(is_directory(*m_pTestee));
  delete m_pTestee;
  m_pTestee = new Path(root);
  EXPECT_FALSE(exists(*m_pTestee));
  EXPECT_FALSE(is_directory(*m_pTestee));
}

TEST_F( PathTest, should_equal ) {
  const std::string root = "aaa/bbb/../../ccc";
  Path* p2=new Path("ccc///////");
//  p2->assign(root);
  m_pTestee->assign(root);
  EXPECT_TRUE(*m_pTestee==*p2);
  
  delete m_pTestee;
  m_pTestee = new Path(root);
  EXPECT_TRUE(*m_pTestee==*m_pTestee);
}
TEST_F( PathTest, should_not_equal ) {
  const std::string root = "aa/";
  Path* p2=new Path("aaa//");
//  p2->assign(root);
  m_pTestee->assign(root);
  EXPECT_TRUE(*m_pTestee!=*p2);
  
  delete m_pTestee;
  m_pTestee = new Path(root);
  EXPECT_TRUE(*m_pTestee!=*p2);
}

TEST_F( PathTest, append_success ) {
  
  const std::string root = "aa/";
  m_pTestee->assign(root);
  m_pTestee->append("aaa");
  string a("aa/aaa");
  EXPECT_TRUE(m_pTestee->string()=="aa/aaa");
  cout << m_pTestee->string()<<endl; 
  delete m_pTestee;
  m_pTestee = new Path("aa/");
  m_pTestee->append("/aaa");
  EXPECT_TRUE(m_pTestee->string()=="aa//aaa");
  delete m_pTestee;
  m_pTestee = new Path("aa");
  m_pTestee->append("/aaa");
  EXPECT_TRUE(m_pTestee->string()=="aa/aaa");
  delete m_pTestee;
  m_pTestee = new Path("aa");
  m_pTestee->append("aaa");
  EXPECT_TRUE(m_pTestee->string()=="aa/aaa");
}

TEST_F( PathTest, should_become_generic_string ) {
  const std::string root = "..///pndk-luba/../pndk-luba/debug//Makefile";
  m_pTestee->assign(root);
  EXPECT_FALSE(is_directory(*m_pTestee));
  EXPECT_TRUE(m_pTestee->generic_string()=="/proj/mtk03931/temp/pndk-luba/debug/Makefile");
  delete m_pTestee;
  m_pTestee = new Path("../pndk-luba/include/..//");
  EXPECT_TRUE(is_directory(*m_pTestee));
  EXPECT_TRUE(m_pTestee->generic_string()=="/proj/mtk03931/temp/pndk-luba/");
}
//function not available
//TEST_F( PathTest, should_erase_redundant_separator) {
//  std::string root = "//aa/";
//  m_pTestee->assign(root);
//  m_pTestee->m_erase_redundant_separator(0);
//  EXPECT_TRUE(m_pTestee->string()=="/aa/");
//  
//  delete m_pTestee;
//  m_pTestee = new Path(root);
//  m_pTestee->m_erase_redundant_separator(0);
//  EXPECT_TRUE(m_pTestee->string()=="/aa/");
//}
