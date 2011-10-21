//===- DirIteratorTest.cpp ------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Support/Directory.h"
#include "DirIteratorTest.h"
#include "errno.h"

using namespace std;
using namespace mcld;
using namespace mcld::sys::fs;
using namespace mcldtest;


// Constructor can do set-up work for all test here.
DirIteratorTest::DirIteratorTest()
{
  //FIXME:Some bugs modifies the global value "errno" to non-zero.
  //      This makes readir() failed when daily build system runs unittest
  //      Remove this after fixing those bugs
  errno = 0;

  // create testee. modify it if need
  m_pDir = new mcld::sys::fs::Directory(".");
}

// Destructor can do clean-up work that doesn't throw exceptions here.
DirIteratorTest::~DirIteratorTest()
{
  delete m_pDir;
}

// SetUp() will be called immediately before each test.
void DirIteratorTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void DirIteratorTest::TearDown()
{
}

//==========================================================================//
// Testcases
//
TEST_F( DirIteratorTest, open_dir ) {
	ASSERT_TRUE( m_pDir->isGood() );

  Directory::iterator entry = m_pDir->begin();
  Directory::iterator enEnd = m_pDir->end();
  static int counter = 0;
  while( entry!=enEnd ) {
    if (0 == entry.path()) {
      cerr << "\tpath is null" << endl;
    }
    else {
      cerr << "\t" <<  counter << ":" << entry.path()->native() << "<EOL>" << endl;
    }

    cerr << "\t\titerator increase" << endl;
    counter++;
    ++entry;
  }
}


