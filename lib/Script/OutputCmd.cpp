//===- OutputCmd.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/OutputCmd.h>
#include <mcld/Support/raw_ostream.h>
#include <mcld/LinkerScript.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// OutputCmd
//===----------------------------------------------------------------------===//
OutputCmd::OutputCmd(const std::string& pOutputFile, LinkerScript& pScript)
  : ScriptCommand(ScriptCommand::Output),
    m_OutputFile(pOutputFile),
    m_Script(pScript)
{
}

OutputCmd::~OutputCmd()
{
}

void OutputCmd::dump() const
{
  mcld::outs() << "OUTPUT ( " << m_OutputFile << " )\n";
}

void OutputCmd::activate()
{
  m_Script.setOutputFile(m_OutputFile);
  // TODO: set the output name if there is no `-o filename' on the cmdline.
  // This option is to define a default name for the output file other than the
  // usual default of a.out.
}

