//===- OutputFormatCmd.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/LinkerScript/OutputFormatCmd.h>
#include <mcld/Support/raw_ostream.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// OutputFormatCmd
//===----------------------------------------------------------------------===//
OutputFormatCmd::OutputFormatCmd(const char* pFormat)
  : ScriptCommand(ScriptCommand::OutputFormat)
{
  m_FormatList.push_back(pFormat);
}

OutputFormatCmd::OutputFormatCmd(const char* pDefault,
                                 const char* pBig,
                                 const char* pLittle)
  : ScriptCommand(ScriptCommand::OutputFormat)
{
  m_FormatList.push_back(pDefault);
  m_FormatList.push_back(pBig);
  m_FormatList.push_back(pLittle);
}

OutputFormatCmd::~OutputFormatCmd()
{
}

void OutputFormatCmd::dump() const
{
  mcld::outs() << "OUTPUT_FORMAT ( ";
  assert(m_FormatList.size() == 1 || m_FormatList.size() == 3);
  for (size_t i = 0; i < m_FormatList.size(); ++i) {
    if (i != 0)
      mcld::outs() << " , ";
    mcld::outs() << m_FormatList[i];
  }
  mcld::outs() << " )\n";
}

void OutputFormatCmd::activate()
{
  // TODO
}

