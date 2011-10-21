//===- MCLDScriptReader.cpp -----------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <MCLDScriptReader.h>

using namespace mcld;

//==========================
// MCLDScriptReader
MCLDScriptReader::MCLDScriptReader( const MCLDInfo& pLDInfo )
: m_LDInfo(pLDInfo)
{
}
