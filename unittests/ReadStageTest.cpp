//===- ReadStageTest.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ReadStageTest.h"
#include <mcld/LD/LDContext.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/SectionData.h>
#include <mcld/LD/Fragment.h>
#include <mcld/LD/FragmentRef.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/ResolveInfo.h>

#include <sstream>
#include <iostream>

using namespace std;

using namespace mcld;
using namespace mcld::test;


// Constructor can do set-up work for all test here.
ReadStageTest::ReadStageTest()
  : m_pLinker(NULL) {
}

// Destructor can do clean-up work that doesn't throw exceptions here.
ReadStageTest::~ReadStageTest()
{
}

// SetUp() will be called immediately before each test.
void ReadStageTest::SetUp()
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
void ReadStageTest::TearDown()
{
  delete m_pLinker;
}

void ReadStageTest::dumpInput(const mcld::Input &pInput, mcld::FileHandle &pFile, size_t pIdent)
{
  stringstream sstream;
  for (int i=0; i < pIdent; ++i)
    sstream << " ";
  sstream << "<input name=\"" << pInput.name() << "\">\n";

  LDContext::const_sect_iterator sect, sectEnd = pInput.context()->sectEnd();
  for (sect = pInput.context()->sectBegin(); sect != sectEnd; ++sect) {
    for (int i=0; i < (pIdent+1); ++i)
      sstream << " ";
    sstream << "<section name=\"" << (*sect)->name() << "\"/>\n";
  }
  for (int i=0; i < pIdent; ++i)
    sstream << " ";
  sstream << "</input>\n";

  size_t org_size = pFile.size();
  pFile.truncate(sstream.str().size() + org_size);
  pFile.write(sstream.str().data(), org_size, sstream.str().size());
}

void ReadStageTest::dumpOutput(const mcld::Output &pOutput, mcld::FileHandle &pFile, size_t pIdent)
{
  stringstream sstream;
  for (int i=0; i < pIdent; ++i)
    sstream << " ";
  sstream << "<output name=\"" << pOutput.name() << "\">\n";

  LDContext::const_sect_iterator sect, sectEnd = pOutput.context()->sectEnd();
  for (sect = pOutput.context()->sectBegin(); sect != sectEnd; ++sect) {
    for (int i=0; i < (pIdent+1); ++i)
      sstream << " ";
    sstream << "<section name=\"" << (*sect)->name() << "\"/>\n";
  }
  for (int i=0; i < pIdent; ++i)
    sstream << " ";
  sstream << "</output>\n";

  size_t org_size = pFile.size();
  pFile.truncate(sstream.str().size() + org_size);
  pFile.write(sstream.str().data(), org_size, sstream.str().size());
}
//===----------------------------------------------------------------------===//
// Testcases
//===----------------------------------------------------------------------===//
TEST_F(ReadStageTest, quake) {
  mcld::sys::fs::Path top_level = TOPDIR;

  // set up output
  m_pLinker->config()->output().setType(mcld::Output::DynObj);
  m_pLinker->setOutput(top_level + "unittests/plasma.so");


  // set up input
  m_pLinker->addObject(top_level + "test/libs/ARM/Android/android-14/crtbegin_so.o");
  m_pLinker->addObject(top_level + "test/Android/Plasma/ARM/plasma.o");
  m_pLinker->addNameSpec("m");
  m_pLinker->addNameSpec("log");
  m_pLinker->addNameSpec("jnigraphics");
  m_pLinker->addNameSpec("c");
  m_pLinker->addObject(top_level + "test/libs/ARM/Android/android-14/crtend_so.o");

  // dump status
  m_pLinker->getDriver()->normalize();

  FileHandle file;
  file.open(top_level + "unittests/read_stage.xml",
     FileHandle::ReadWrite | FileHandle::Create | FileHandle::Truncate, 0644);

  InputTree::iterator input, inEnd = m_pLinker->config()->inputs().end();
  for (input = m_pLinker->config()->inputs().begin(); input != inEnd; ++input) {
    dumpInput(**input, file, 1);
  }

  dumpOutput(m_pLinker->config()->output(), file, 1);
  // dump status
  ASSERT_TRUE(m_pLinker->getDriver()->mergeSections());
}

