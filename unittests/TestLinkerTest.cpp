//===- TestLinkerTest.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "Linker/TestLinker.h"
#include "TestLinkerTest.h"

#include <mcld/Support/Path.h>
#include <mcld/MC/MCLDDirectory.h>

using namespace mcld;
using namespace mcld::test;
using namespace mcld::sys::fs;
using namespace mcldtest;


// Constructor can do set-up work for all test here.
TestLinkerTest::TestLinkerTest()
  : m_pLinker(NULL) {
}

// Destructor can do clean-up work that doesn't throw exceptions here.
TestLinkerTest::~TestLinkerTest()
{
}

// SetUp() will be called immediately before each test.
void TestLinkerTest::SetUp()
{
  m_pLinker = new mcld::test::TestLinker();
  m_pLinker->initialize("arm-none-linux-gnueabi");

  // set up target-dependent constraints of attributes
  m_pLinker->config()->attrFactory().constraint().enableWholeArchive();
  m_pLinker->config()->attrFactory().constraint().disableAsNeeded();
  m_pLinker->config()->attrFactory().constraint().setSharedSystem();

  // set up the predefined attributes
  m_pLinker->config()->attrFactory().predefined().setWholeArchive();
  m_pLinker->config()->attrFactory().predefined().setDynamic();

  // set up target dependent options
  mcld::sys::fs::Path path = TOPDIR;
  path.append("test/libs/ARM/Android/android-14");
  m_pLinker->setSysRoot(path);
  m_pLinker->addSearchDir("=/");

  m_pLinker->config()->options().setDyld("/usr/lib/ld.so.1");
  m_pLinker->config()->options().setBsymbolic(true);
}

// TearDown() will be called immediately after each test.
void TestLinkerTest::TearDown()
{
  delete m_pLinker;
}

//===----------------------------------------------------------------------===//
// Testcases
//===----------------------------------------------------------------------===//
TEST_F( TestLinkerTest, test) {
  m_pLinker->config()->options().setVerbose(3);
  mcld::sys::fs::Path top_level = TOPDIR;
  m_pLinker->addObject(top_level + "test/libs/ARM/Android/android-14/crtbegin_so.o");
  m_pLinker->addObject(top_level + "test/Android/Plasma/ARM/plasma.o");
  m_pLinker->addNameSpec("m");
  m_pLinker->addNameSpec("log");
  m_pLinker->addNameSpec("jnigraphics");
  m_pLinker->addNameSpec("c");
  m_pLinker->addObject(top_level + "test/libs/ARM/Android/android-14/crtend_so.o");
}

