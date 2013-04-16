//===- OutputArchCmd.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/LinkerScript/OutputArchCmd.h>
#include <mcld/Support/raw_ostream.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// OutputArchCmd
//===----------------------------------------------------------------------===//
OutputArchCmd::OutputArchCmd(const std::string& pArch)
  : ScriptCommand(ScriptCommand::OutputArch),
    m_Arch(pArch)
{
}

OutputArchCmd::~OutputArchCmd()
{
}

void OutputArchCmd::dump() const
{
  mcld::outs() << "OUTPUT_ARCH ( " << m_Arch << " )\n";
}

void OutputArchCmd::activate()
{
  // TODO
}

