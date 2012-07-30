//===- ReadStageTest.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_UNITTEST_READSTAGE_TEST_H
#define MCLD_UNITTEST_READSTAGE_TEST_H

#include <gtest.h>
#include "Linker/TestLinker.h"

namespace mcld {
namespace test {

class ReadStageTest : public ::testing::Test
{
public:
  // Constructor can do set-up work for all test here.
  ReadStageTest();

  // Destructor can do clean-up work that doesn't throw exceptions here.
  virtual ~ReadStageTest();

  // SetUp() will be called immediately before each test.
  virtual void SetUp();

  // TearDown() will be called immediately after each test.
  virtual void TearDown();

  void dumpInput(const mcld::Input &pInput, mcld::FileHandle &pFile, size_t pIdent);

  void dumpOutput(const mcld::Output &pOutput, mcld::FileHandle &pFile, size_t pIdent);
protected:
  TestLinker* m_pLinker;
};

} // end of namespace test
} // end of namespace mcld

#endif

