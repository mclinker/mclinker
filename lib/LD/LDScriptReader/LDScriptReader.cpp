//===- MCLDScriptReader.cpp -----------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "LDScriptReader.h"

namespace mcld {

// MCLDScriptReader
MCLDScriptReader::MCLDScriptReader( const MCLDInfo& pLDInfo )
: m_LDInfo(pLDInfo)
{
}

} //end namespace mcld
