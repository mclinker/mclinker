/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/


#include <mcld/Support/FileSystem.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MemoryAreaFactory.h>
#include <mcld/Support/Path.h>
#include <iostream>

#include "MemoryAreaTest.h"

#include <fcntl.h>

using namespace std;
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

