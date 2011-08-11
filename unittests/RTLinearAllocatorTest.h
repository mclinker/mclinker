/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Reloc.h <nowar100@gmail.com>                                            *
 ****************************************************************************/
#ifndef RTLINEARALLOCATOR_TEST_H
#define RTLINEARALLOCATOR_TEST_H

#include <gtest.h>
#include <mcld/ADT/Allocators.h>

namespace mcldtest
{

/** \class RTLinearAllocatorTest
 *  \brief 
 *
 *  \see RTLinearAllocator 
 */
class RTLinearAllocatorTest : public ::testing::Test
{
public:
	// Constructor can do set-up work for all test here.
	RTLinearAllocatorTest();

	// Destructor can do clean-up work that doesn't throw exceptions here.
	virtual ~RTLinearAllocatorTest();

	// SetUp() will be called immediately before each test.
	virtual void SetUp();

	// TearDown() will be called immediately after each test.
	virtual void TearDown();

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
  enum {  CHUNK_SIZE = 32  };

protected:
	mcld::RTLinearAllocator<Data>* m_pTestee;
};

} // namespace of mcldtest

#endif

