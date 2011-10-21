//===- StringUnorderedMapTest.h -------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef STRINGUNORDEREDMAP_TEST_H
#define STRINGUNORDEREDMAP_TEST_H

#include "mcld/LD/StringUnorderedMap.h"
#include <string>
#include <gtest.h>

namespace mcld
{

} // namespace for mcld

namespace mcldtest
{

/** \class StringUnorderedMapTest
 *  \brief
 *
 *  \see StringUnorderedMap
 */
class StringUnorderedMapTest : public ::testing::Test
{
public:
	// Constructor can do set-up work for all test here.
	StringUnorderedMapTest();

	// Destructor can do clean-up work that doesn't throw exceptions here.
	virtual ~StringUnorderedMapTest();

	// SetUp() will be called immediately before each test.
	virtual void SetUp();

	// TearDown() will be called immediately after each test.
	virtual void TearDown();

protected:
	mcld::StringUnorderedMap<int, std::string>* m_pTestee;
};

} // namespace of mcldtest

#endif

