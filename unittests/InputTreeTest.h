//===- InputTreeTest.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef INPUTTREE_TEST_H
#define INPUTTREE_TEST_H

#include <gtest.h>

namespace mcld
{
class InputTree;
class InputFactory;
class AttributeFactory;

} // namespace for mcld

namespace mcldtest
{

/** \class InputTreeTest
 *  \brief 
 *
 *  \see InputTree 
 */
class InputTreeTest : public ::testing::Test
{
public:
	// Constructor can do set-up work for all test here.
	InputTreeTest();

	// Destructor can do clean-up work that doesn't throw exceptions here.
	virtual ~InputTreeTest();

	// SetUp() will be called immediately before each test.
	virtual void SetUp();

	// TearDown() will be called immediately after each test.
	virtual void TearDown();

protected:
	mcld::AttributeFactory *m_pAttr;
	mcld::InputFactory *m_pAlloc;
	mcld::InputTree* m_pTestee;
};

} // namespace of mcldtest

#endif

