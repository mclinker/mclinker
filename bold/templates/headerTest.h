/*****************************************************************************
 *   Test Suite of The BOLD Project,                                         *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   ${AUTHOR} <${EMAIL}>                                                    *
 ****************************************************************************/
#ifndef ${CLASS_NAME}_TEST_H
#define ${CLASS_NAME}_TEST_H

#include <gtest.h>

namespace BOLD
{
class ${class_name};

} // namespace for BOLD

namespace BOLDTEST
{

/** \class ${class_name}Test
 *  \brief ${brief}
 *
 *  \see ${class_name} 
 */
class ${class_name}Test : public ::testing::Test
{
public:
	// Constructor can do set-up work for all test here.
	${class_name}Test();

	// Destructor can do clean-up work that doesn't throw exceptions here.
	virtual ~${class_name}Test();

	// SetUp() will be called immediately before each test.
	virtual void SetUp();

	// TearDown() will be called immediately after each test.
	virtual void TearDown();

private:
	BOLD::${class_name}* m_pTestee;
};

/** remove the mark when you starts writting testcases

TEST_F( ${class_name}Test, ${member function of ${class_name}} ) {
	Write you exercise here
}

**/

} // namespace of BOLDTEST

#endif

