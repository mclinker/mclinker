//===- StrSymPoolTest.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef STRSYMPOOL_TEST_H
#define STRSYMPOOL_TEST_H

#include <gtest.h>

namespace mcld
{
class StrSymPool;

} // namespace for mcld

namespace mcldtest
{

/** \class StrSymPoolTest
 *  \brief 
 *
 *  \see StrSymPool 
 */
class StrSymPoolTest : public ::testing::Test
{
public:
	// Constructor can do set-up work for all test here.
	StrSymPoolTest();

	// Destructor can do clean-up work that doesn't throw exceptions here.
	virtual ~StrSymPoolTest();

	// SetUp() will be called immediately before each test.
	virtual void SetUp();

	// TearDown() will be called immediately after each test.
	virtual void TearDown();

protected:
	mcld::StrSymPool* m_pTestee;
};

} // namespace of mcldtest

#endif

