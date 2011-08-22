/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_DIR_ITERATOR_TEST_H
#define MCLD_DIR_ITERATOR_TEST_H

#include <gtest.h>

namespace mcld {
namespace sys {
namespace fs {
class Directory;
class DirIterator;
}
}
} // namespace for mcld

namespace mcldtest
{

/** \class DirIteratorTest
 *  \brief 
 *
 *  \see DirIterator 
 */
class DirIteratorTest : public ::testing::Test
{
public:
	// Constructor can do set-up work for all test here.
	DirIteratorTest();

	// Destructor can do clean-up work that doesn't throw exceptions here.
	virtual ~DirIteratorTest();

	// SetUp() will be called immediately before each test.
	virtual void SetUp();

	// TearDown() will be called immediately after each test.
	virtual void TearDown();
protected:
	mcld::sys::fs::Directory *m_pDir;
};

} // namespace of mcldtest

#endif

