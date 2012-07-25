//===- TestLinkerTest.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TEST_LINKER_TEST_H
#define MCLD_TEST_LINKER_TEST_H
#include <gtest.h>

#include "Linker/TestLinker.h"

namespace mcldtest
{

class TestLinkerTest : public ::testing::Test
{
public:
  // Constructor can do set-up work for all test here.
  TestLinkerTest();

  // Destructor can do clean-up work that doesn't throw exceptions here.
  virtual ~TestLinkerTest();

  // SetUp() will be called immediately before each test.
  virtual void SetUp();

  // TearDown() will be called immediately after each test.
  virtual void TearDown();

protected:
  mcld::test::TestLinker* m_pLinker;
};

} // namespace of mcldtest

#endif

