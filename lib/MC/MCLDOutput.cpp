//===- MCLDOutput.cpp -----------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/MC/MCLDOutput.h"

using namespace mcld;

//==========================
// MCLDOutput
Output::Output()
  : MCLDFile("output") {
}

Output::Output(const sys::fs::Path& pRealPath,
               Output::Type pType)
  : MCLDFile("output", pRealPath, pType) {
}
                  
Output::~Output()
{
}

