//===- headerTest.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_${CLASS_NAME}_TEST_H
#define MCLD_${CLASS_NAME}_TEST_H

#include <gtest.h>

namespace mcld
{
class ${class_name};

} // namespace for mcld

namespace mcldtest
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

protected:
	mcld::${class_name}* m_pTestee;
};

} // namespace of mcldtest

#endif

