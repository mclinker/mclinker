/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef STRINGUNORDEREDMAP_TEST_H
#define STRINGUNORDEREDMAP_TEST_H

#include <mcld/LD/StringUnorderedMap.h>
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

