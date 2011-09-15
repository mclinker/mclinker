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

#include "MemoryAreaTest.h"

#include <fcntl.h>

using namespace mcld;
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

TEST_F( MemoryAreaTest, read_file ) {
  unsigned char range[16];
  MemoryAreaFactory *AreaFactory = new MemoryAreaFactory(1);

  sys::fs::Path Path("/lib/libc.so.6");
  m_pTestee = AreaFactory->produce(Path.c_str() ,O_RDONLY);

  MemoryRegion *Region;
  Region = m_pTestee->request(0, sizeof(range));

  if (!m_pTestee->isGood())
    FAIL();

  const sys::fs::detail::Address magic = Region->getBuffer();

  if (magic){
    if (magic[0] == 0x7F && magic[1] == 'E' &&
        magic[2] == 'L' && magic[3] == 'F')
      SUCCEED();
    else
      FAIL();
  }

  else
    FAIL();

  delete AreaFactory;
}

