/*****************************************************************************
 *   The BOLD Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@gmail.com>                                          *
 ****************************************************************************/
#ifdef ENABLE_UNITTEST
#include <gtest.h>

int unit_test( int argc, char* argv[] )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}

#endif

int main( int argc, char* argv[] )
{

#ifdef ENABLE_UNITTEST
	// since the command line interface is not written, always call 
	// unit-test()
	return unit_test( argc, argv );
#endif

}

