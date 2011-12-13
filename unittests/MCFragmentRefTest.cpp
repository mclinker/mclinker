//===- MCFragmentRefTest --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "mcld/MC/MCFragmentRef.h"
#include "mcld/MC/MCRegionFragment.h"
#include "mcld/Support/MemoryAreaFactory.h"
#include "mcld/Support/Path.h"
#include "MCFragmentRefTest.h"

using namespace mcld;
using namespace mcld::sys::fs;
using namespace mcld::sys::fs::detail;
using namespace mcldtest;

// Constructor can do set-up work for all test here.
MCFragmentRefTest::MCFragmentRefTest()
{
}

// Destructor can do clean-up work that doesn't throw exceptions here.
MCFragmentRefTest::~MCFragmentRefTest()
{
}

// SetUp() will be called immediately before each test.
void MCFragmentRefTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void MCFragmentRefTest::TearDown()
{
}

//==========================================================================//
// Testcases
//
TEST_F( MCFragmentRefTest, ) {
  Path path(TOPDIR);
  path.append("unittests/test3.txt");
  MemoryAreaFactory* areaFactory = new MemoryAreaFactory(1);
  MemoryArea* area = areaFactory->produce(path, MemoryArea::ReadWrite);

  MemoryRegion* region = area->request(0, 4096);
  MCRegionFragment *frag = new MCRegionFragment(*region);
  MCFragmentRef *ref = new MCFragmentRef(*frag);

  ASSERT_EQ('H', region->getBuffer()[0]);
  ASSERT_EQ(4096, region->size());
  ASSERT_EQ('H', frag->getRegion().getBuffer()[0]);
  ASSERT_EQ(4096, frag->getRegion().size());
  ASSERT_EQ(frag, ref->frag());
  ASSERT_EQ('H', static_cast<MCRegionFragment*>(ref->frag())->getRegion().getBuffer()[0]);
  ASSERT_EQ(4096, static_cast<MCRegionFragment*>(ref->frag())->getRegion().size());
  ASSERT_EQ('H', ref->deref()[0]);

  ASSERT_FALSE(llvm::MCDataFragment::classof(frag));
  ASSERT_TRUE(MCRegionFragment::classof(frag));

  delete ref;
  delete frag;
  delete areaFactory;
}

