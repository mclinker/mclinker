//===- MCLDOutput.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
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
  : MCLDFile("") {
}

Output::Output(const sys::fs::Path& pRealPath,
               Output::Type pType)
  : MCLDFile("", pRealPath, pType) {
}
                  
Output::~Output()
{
}

void Output::setSOName(const std::string& pName)
{
  size_t pos = pName.find_last_of(sys::fs::separator);
  if (std::string::npos == pos)
    m_Name = pName;
  else
    m_Name = pName.substr(pos + 1);
}

