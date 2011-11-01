//===- MCLDScriptReader.h -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SCRIPT_READER_H
#define MCLD_SCRIPT_READER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class MCLDScriptReader
 *
 */
class MCLDScriptReader
{
public:
  MCLDScriptReader( const MCLDInfo& LDInfo );

  MCLDCommand* createCommand();
  MCLDLayout* createLayout();
private:
  bool hasCommand() const;
private:
  const MCLDInfo& m_LDInfo;
};

} // namespace of BOLD

#endif

