//===- LinkerTest.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "LinkerTest.h"

#include <mcld/Environment.h>
#include <mcld/Module.h>
#include <mcld/InputTree.h>
#include <mcld/IRBuilder.h>
#include <mcld/Linker.h>

using namespace mcld;
using namespace mcld::test;


// Constructor can do set-up work for all test here.
LinkerTest::LinkerTest()
{
}

// Destructor can do clean-up work that doesn't throw exceptions here.
LinkerTest::~LinkerTest()
{
}

// SetUp() will be called immediately before each test.
void LinkerTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void LinkerTest::TearDown()
{
}

//===----------------------------------------------------------------------===//
// Testcases
//===----------------------------------------------------------------------===//
TEST_F( LinkerTest, set_up_n_clean_up) {
  Initialize();
  Finalize();
}

