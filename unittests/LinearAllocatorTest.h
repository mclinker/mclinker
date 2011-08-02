/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef LINEARALLOCATOR_TEST_H
#define LINEARALLOCATOR_TEST_H

#include <gtest.h>
#include <mcld/ADT/Allocators.h>

namespace mcldtest
{

/** \class LinearAllocatorTest
 *  \brief The testcase for LinearAllocator
 *
 *  \see LinearAllocator 
 */
class LinearAllocatorTest : public ::testing::Test
{
public:
	struct Data {
		Data()
		: one(1), two(2), three(3), four(4)
		{ }

		Data( unsigned int pOne, unsigned int pTwo, unsigned char pThree, unsigned char pFour)
		{
			one = pOne;
			two = pTwo;
			three = pThree;
			four = pFour;
		}

		~Data()
		{
			one = -1;
			two = -2;
			three = -3;
			four = -4;
		}

		unsigned int one;
		unsigned int two;
		unsigned char three;
		unsigned char four;
	};
public:
	// Constructor can do set-up work for all test here.
	LinearAllocatorTest();

	// Destructor can do clean-up work that doesn't throw exceptions here.
	virtual ~LinearAllocatorTest();

	// SetUp() will be called immediately before each test.
	virtual void SetUp();

	// TearDown() will be called immediately after each test.
	virtual void TearDown();

protected:
	enum { CHUNK_SIZE = 32 };
	typedef mcld::LinearAllocator<Data, CHUNK_SIZE> Alloc;
protected:
	Alloc* m_pTestee;
};

} // namespace of mcldtest

#endif

