/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef LDSYMBOL_TEST_H
#define LDSYMBOL_TEST_H

#include <gtest.h>

namespace mcld
{
class LDSymbol;

} // namespace for mcld

namespace mcldtest
{

/** \class LDSymbolTest
 */
class LDSymbolTest : public ::testing::Test
{
public:
  // Constructor can do set-up work for all test here.
  LDSymbolTest();

  // Destructor can do clean-up work that doesn't throw exceptions here.
  virtual ~LDSymbolTest();

  // SetUp() will be called immediately before each test.
  virtual void SetUp();

  // TearDown() will be called immediately after each test.
  virtual void TearDown();

};

} // namespace of mcldtest

#endif

