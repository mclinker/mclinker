//===- MemoryAreaTest.cpp -------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//


#include "mcld/Support/FileSystem.h"
#include "mcld/Support/MemoryArea.h"
#include "mcld/Support/MemoryRegion.h"
#include "mcld/Support/MemoryAreaFactory.h"
#include "mcld/Support/Path.h"

#include "MemoryAreaTest.h"
#include <fcntl.h>

using namespace mcld;
using namespace mcld::sys::fs;
using namespace mcldtest;


// Constructor can do set-up work for all test here.
MemoryAreaTest::MemoryAreaTest()
{
}

// Destructor can do clean-up work that doesn't throw exceptions here.
MemoryAreaTest::~MemoryAreaTest()
{
}

// SetUp() will be called immediately before each test.
void MemoryAreaTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void MemoryAreaTest::TearDown()
{
}

//==========================================================================//
// Testcases
//
TEST_F( MemoryAreaTest, should_fail_to_write_file ) {
  unsigned char range[16];
  MemoryAreaFactory *AreaFactory = new MemoryAreaFactory(1);

  sys::fs::Path Path("/lib/libc.so.6");
  m_pTestee = AreaFactory->produce(Path.c_str() ,O_WRONLY);
  ASSERT_FALSE(m_pTestee->isMapped());
}

TEST_F( MemoryAreaTest, read_file ) {
  unsigned char range[16];
  MemoryAreaFactory *AreaFactory = new MemoryAreaFactory(1);

  sys::fs::Path Path("/lib/libc.so.6");
  m_pTestee = AreaFactory->produce(Path.c_str() ,O_RDONLY);

  if (!m_pTestee->isMapped())
    FAIL() << "file is not open.";
  if (!m_pTestee->isGood())
    FAIL() << "file is failed to open";

  MemoryRegion* region = m_pTestee->request(0, sizeof(range));
  if (!region)
    FAIL() << "region is NULL";

  const sys::fs::detail::Address magic = region->getBuffer();

  if (magic){
    if (magic[0] == 0x7F && magic[1] == 'E' &&
        magic[2] == 'L' && magic[3] == 'F')
      SUCCEED();
    else
      FAIL() << "magic number is not correctly read";
  }
  else
    FAIL() << "magic is NULL";

  delete AreaFactory;
}

TEST_F( MemoryAreaTest, read_a_ascii_file )
{
	Path path(TOPDIR);
	path.append("unittests/test.txt");
	MemoryAreaFactory *AreaFactory = new MemoryAreaFactory(1);
	MemoryArea* area = AreaFactory->produce(path, O_RDONLY);
	MemoryRegion* region = area->request(3, 2);
	ASSERT_EQ('s', region->getBuffer()[0]);
	ASSERT_EQ(' ', region->getBuffer()[1]);
	delete AreaFactory;
}

TEST_F( MemoryAreaTest, write_a_ascii_file )
{
	Path path(TOPDIR);
	path.append("unittests/test2.txt");
	MemoryAreaFactory *AreaFactory = new MemoryAreaFactory(1);
	MemoryArea* area = AreaFactory->produce(path, O_RDWR);
        ASSERT_TRUE(area->isMapped());
	MemoryRegion* region = area->request(3, 4);
	region->getBuffer()[0] = 'L';
	region->getBuffer()[1] = 'u';
	region->getBuffer()[2] = 'b';
	region->getBuffer()[3] = 'a';
	region->sync();
	area->unmap();
        area->map(path, O_RDONLY);
        ASSERT_TRUE(area->isMapped());
        ASSERT_TRUE(area->isGood());
        region = area->request(5, 2);
        ASSERT_EQ('b', region->getBuffer()[0]);
        ASSERT_EQ('a', region->getBuffer()[1]);
	delete AreaFactory;
}

TEST_F( MemoryAreaTest, write_a_ascii_file_2 )
{
	Path path(TOPDIR);
	path.append("unittests/test2.txt");
	MemoryAreaFactory *AreaFactory = new MemoryAreaFactory(1);
	MemoryArea* area = AreaFactory->produce(path, O_RDWR);
        ASSERT_TRUE(area->isMapped());
        ASSERT_TRUE(area->isGood());
	MemoryRegion* region = area->request(3, 4);
	region->getBuffer()[0] = 'L';
	region->getBuffer()[1] = 'u';
	region->getBuffer()[2] = 'b';
	region->getBuffer()[3] = 'a';
	region->sync();
	area->unmap();
        area->map(path, O_RDONLY);
        ASSERT_TRUE(area->isMapped());
        ASSERT_TRUE(area->isGood());
        region = area->request(5, 2);
        ASSERT_EQ('b', region->getBuffer()[0]);
        ASSERT_EQ('a', region->getBuffer()[1]);
	delete AreaFactory;
}

TEST_F( MemoryAreaTest, read_one_page )
{
        Path path(TOPDIR) ;
	path.append("unittests/test3.txt") ;
	MemoryAreaFactory *AreaFactory = new MemoryAreaFactory(1) ;
	MemoryArea* area = AreaFactory->produce(path, O_RDWR) ;
	ASSERT_TRUE(area->isMapped()) ;
	ASSERT_TRUE(area->isGood()) ;
	MemoryRegion* region = area->request(0, 4096) ;
	ASSERT_EQ('H', region->getBuffer()[0]);
	ASSERT_EQ('E', region->getBuffer()[1]);
	delete AreaFactory ;
}

TEST_F( MemoryAreaTest, write_one_page )
{
        Path path(TOPDIR) ;
	path.append("unittests/test3.txt") ;
        MemoryAreaFactory *AreaFactory = new MemoryAreaFactory(1) ;
	MemoryArea* area = AreaFactory->produce(path, O_RDWR) ;
	ASSERT_TRUE(area->isMapped()) ;
	ASSERT_TRUE(area->isGood()) ;
	MemoryRegion* region = area->request(0, 4096) ;
	region->getBuffer()[4000] = 'K' ;
	region->getBuffer()[4001] = 'R' ;
	region->sync() ;
	area->unmap() ;
	area->map(path, O_RDONLY) ;
	region = area->request( 4000, 4) ;
	ASSERT_EQ('K', region->getBuffer()[0]);
	ASSERT_EQ('R', region->getBuffer()[1]);
	region->getBuffer()[0] = 'O' ;
	region->getBuffer()[1] = 'H' ;
        region->sync() ;
	delete AreaFactory ;
}

TEST_F( MemoryAreaTest, write_sync )
{
        Path path(TOPDIR) ;
	path.append("unittests/test3.txt") ;
	MemoryAreaFactory *AreaFactory = new MemoryAreaFactory(1) ; 
	MemoryArea* area = AreaFactory->produce(path, O_RDWR) ;
	ASSERT_TRUE(area->isMapped()) ;
	ASSERT_TRUE(area->isGood()) ;
	MemoryRegion* region1 = area->request(0, 4096) ;
	MemoryRegion* region2 = area->request(512, 1024) ;
	region1->getBuffer()[1000] = 'X' ;
	region1->getBuffer()[1001] = 'D' ;
	region2->getBuffer()[488] = ':' ;
	region2->getBuffer()[489] = ')' ;
	region1->sync() ;
	area->unmap();
	area->map(path, O_RDWR) ;
	region1 = area->request(0, 1024) ;
	EXPECT_EQ(':', region1->getBuffer()[1000]) ;
	EXPECT_EQ(')', region1->getBuffer()[1001]) ;
	region1->getBuffer()[1000] = '@' ;
	region1->getBuffer()[1001] = '@' ;
	region1->sync();
	delete AreaFactory ;
}

   

