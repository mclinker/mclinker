//===- LinkerTest.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "LinkerTest.h"

#include <mcld/Environment.h>
#include <mcld/Module.h>
#include <mcld/InputTree.h>
#include <mcld/IRBuilder.h>
#include <mcld/Linker.h>
#include <mcld/LinkerConfig.h>

#include <mcld/Support/Path.h>

using namespace mcld;
using namespace mcld::test;
using namespace mcld::sys::fs;


// Constructor can do set-up work for all test here.
LinkerTest::LinkerTest()
{
}

// Destructor can do clean-up work that doesn't throw exceptions here.
LinkerTest::~LinkerTest()
{
}

// SetUp() will be called immediately before each test.
void LinkerTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void LinkerTest::TearDown()
{
}

//===----------------------------------------------------------------------===//
// Testcases
//===----------------------------------------------------------------------===//
TEST_F( LinkerTest, set_up_n_clean_up) {

  Initialize();
  LinkerConfig config("arm-none-linux-gnueabi");

  Module module("test");
  config.setCodeGenType(LinkerConfig::DynObj);

  Linker linker;
  linker.config(config);

  IRBuilder builder(module, config);
  // create inputs here
  //   builder.CreateInput("./test.o");

  if (linker.link(module, builder))
    linker.emit("./test.so");

  Finalize();
}

// %MCLinker --shared -soname=libplasma.so -Bsymbolic \
// -mtriple="armv7-none-linux-gnueabi" \
// -L=%p/../../../libs/ARM/Android/android-14 \
// %p/../../../libs/ARM/Android/android-14/crtbegin_so.o \
// %p/plasma.o \
// -lm -llog -ljnigraphics -lc  \
// %p/../../../libs/ARM/Android/android-14/crtend_so.o \
// -o libplasma.so
TEST_F( LinkerTest, plasma) {

  Initialize();

  LinkerConfig config;
  config.setCodeGenType(LinkerConfig::DynObj);  ///< --shared
  config.options().setSOName("libplasma.so");   ///< --soname=libplasma.so
  config.options().setBsymbolic();           ///< -Bsymbolic
  config.setTriple("armv7-none-linux-gnueabi"); ///< --mtriple="armv7-none-linux-gnueabi"

  /// -L=${TOPDIR}/test/libs/ARM/Android/android-14
  Path search_dir(TOPDIR);
  search_dir.append("test/libs/ARM/Android/android-14");
  config.options().directories().insert(search_dir);

  Module module("plasma");
  IRBuilder builder(module, config);

  /// ${TOPDIR}/test/libs/ARM/Android/android-14/crtbegin_so.o
  Path crtbegin(search_dir);
  crtbegin.append("crtbegin_so.o");
  builder.ReadInput("crtbegin", crtbegin);

  /// ${TOPDIR}/test/Android/Plasma/ARM/plasma.o
  Path plasma(TOPDIR);
  plasma.append("test/Android/Plasma/ARM/plasma.o");
  builder.ReadInput("plasma", plasma);

  // -lm -llog -ljnigraphics -lc
  builder.ReadInput("m");
  builder.ReadInput("log");
  builder.ReadInput("jnigraphics");
  builder.ReadInput("c");

  /// ${TOPDIR}/test/libs/ARM/Android/android-14/crtend_so.o
  Path crtend(search_dir);
  crtend.append("crtend_so.o");
  builder.ReadInput("crtend", crtend);

  Linker linker;
  linker.config(config);

  if (linker.link(module, builder)) {
    linker.emit("libplasma.so"); ///< -o libplasma.so
  }

  Finalize();
}

