//===- OutputSectDesc.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/OutputSectDesc.h>
#include <mcld/Script/RpnExpr.h>
#include <mcld/Script/ScriptInput.h>
#include <mcld/Script/StrToken.h>
#include <mcld/Support/raw_ostream.h>
#include <llvm/Support/Casting.h>
#include <cassert>

using namespace mcld;

//===----------------------------------------------------------------------===//
// OutputSectDesc
//===----------------------------------------------------------------------===//
OutputSectDesc::OutputSectDesc(const std::string& pName, const Prolog& pProlog)
  : ScriptCommand(ScriptCommand::OutputSectDesc),
    m_Name(pName),
    m_Prolog(pProlog)
{
}

OutputSectDesc::~OutputSectDesc()
{
  for (iterator it = begin(), ie = end(); it != ie; ++it) {
    if (*it != NULL)
      delete *it;
  }
}

bool OutputSectDesc::hasVMA() const
{
  return m_Prolog.vma != NULL && !m_Prolog.vma->empty();
}

RpnExpr& OutputSectDesc::vma()
{
  assert(hasVMA());
  return *m_Prolog.vma;
}

const RpnExpr& OutputSectDesc::vma() const
{
  assert(hasVMA());
  return *m_Prolog.vma;
}

OutputSectDesc::Type OutputSectDesc::type() const
{
  return m_Prolog.type;
}

bool OutputSectDesc::hasLMA() const
{
  return m_Prolog.lma != NULL && !m_Prolog.lma->empty();
}

RpnExpr& OutputSectDesc::lma()
{
  assert(hasLMA());
  return *m_Prolog.lma;
}

const RpnExpr& OutputSectDesc::lma() const
{
  assert(hasLMA());
  return *m_Prolog.lma;
}

bool OutputSectDesc::hasAlign() const
{
  return m_Prolog.align != NULL && !m_Prolog.align->empty();
}

RpnExpr& OutputSectDesc::align()
{
  assert(hasAlign());
  return *m_Prolog.align;
}

const RpnExpr& OutputSectDesc::align() const
{
  assert(hasAlign());
  return *m_Prolog.align;
}

bool OutputSectDesc::hasSubAlign() const
{
  return m_Prolog.sub_align != NULL && !m_Prolog.sub_align->empty();
}

RpnExpr& OutputSectDesc::subAlign()
{
  assert(hasSubAlign());
  return *m_Prolog.sub_align;
}

const RpnExpr& OutputSectDesc::subAlign() const
{
  assert(hasSubAlign());
  return *m_Prolog.sub_align;
}

OutputSectDesc::Constraint OutputSectDesc::constraint() const
{
  return m_Prolog.constraint;
}

bool OutputSectDesc::hasRegion() const
{
  return m_Epilog.region != NULL;
}

const std::string& OutputSectDesc::region() const
{
  assert(hasRegion());
  return *m_Epilog.region;
}

bool OutputSectDesc::hasLMARegion() const
{
  return m_Epilog.lma_region != NULL;
}

const std::string& OutputSectDesc::lmaRegion() const
{
  assert(hasLMARegion());
  return *m_Epilog.lma_region;
}

bool OutputSectDesc::hasPhdrs() const
{
  return m_Epilog.phdrs != NULL && !m_Epilog.phdrs->empty();
}

ScriptInput& OutputSectDesc::phdrs()
{
  assert(hasPhdrs());
  return *m_Epilog.phdrs;
}

const ScriptInput& OutputSectDesc::phdrs() const
{
  assert(hasPhdrs());
  return *m_Epilog.phdrs;
}

bool OutputSectDesc::hasFillExp() const
{
  return m_Epilog.fill_exp != NULL && !m_Epilog.fill_exp->empty();
}

RpnExpr& OutputSectDesc::fillExp()
{
  assert(hasFillExp());
  return *m_Epilog.fill_exp;
}

const RpnExpr& OutputSectDesc::fillExp() const
{
  assert(hasFillExp());
  return *m_Epilog.fill_exp;
}

void OutputSectDesc::dump() const
{
  mcld::outs() << m_Name << "\t";

  if (hasVMA()) {
    vma().dump();
    mcld::outs() << "\t";
  }

  switch (type()) {
  case NOLOAD:
    mcld::outs() << "NOLOAD";
    break;
  case DSECT:
    mcld::outs() << "DSECT";
    break;
  case COPY:
    mcld::outs() << "COPY";
    break;
  case INFO:
    mcld::outs() << "INFO";
    break;
  case OVERLAY:
    mcld::outs() << "OVERLAY";
    break;
  default:
    break;
  }
  mcld::outs() << ":\n";

  if (hasLMA()) {
    mcld::outs() << "\tAT ( ";
    lma().dump();
    mcld::outs() << " )\n";
  }

  if (hasAlign()) {
    mcld::outs() << "\tALIGN ( ";
    align().dump();
    mcld::outs() << " )\n";
  }

  if (hasSubAlign()) {
    mcld::outs() << "\tSUBALIGN ( ";
    subAlign().dump();
    mcld::outs() << " )\n";
  }

  switch (constraint()) {
  case ONLY_IF_RO:
    mcld::outs() << "\tONLY_IF_RO\n";
    break;
  case ONLY_IF_RW:
    mcld::outs() << "\tONLY_IF_RW\n";
    break;
  default:
    break;
  }

  mcld::outs() << "\t{\n";
  for (const_iterator it = begin(), ie = end(); it != ie; ++it) {
    switch ((*it)->getKind()) {
    case ScriptCommand::Entry:
    case ScriptCommand::Assignment:
    case ScriptCommand::InputSectDesc:
      mcld::outs() << "\t\t";
      (*it)->dump();
      break;
    default:
      assert(0);
      break;
    }
  }
  mcld::outs() << "\t}";

  if (hasRegion())
    mcld::outs() << "\t>" << region();
  if (hasLMARegion())
    mcld::outs() << "\tAT>" << lmaRegion();

  if (hasPhdrs()) {
    for (ScriptInput::const_iterator it = phdrs().begin(), ie = phdrs().end();
      it != ie; ++it) {
      assert((*it)->kind() == StrToken::String);
      mcld::outs() << ":" << (*it)->name() << " ";
    }
  }

  if (hasFillExp()) {
    mcld::outs() << "= ";
    fillExp().dump();
  }
  mcld::outs() << "\n";
}

void OutputSectDesc::push_back(ScriptCommand* pCommand)
{
  switch (pCommand->getKind()) {
  case ScriptCommand::Assignment:
  case ScriptCommand::InputSectDesc:
    m_OutputSectCmds.push_back(pCommand);
    break;
  default:
    assert(0);
    break;
  }
}

void OutputSectDesc::setEpilog(const Epilog& pEpilog)
{
  m_Epilog.region = pEpilog.region;
  m_Epilog.lma_region = pEpilog.lma_region;
  m_Epilog.phdrs = pEpilog.phdrs;
  m_Epilog.fill_exp = pEpilog.fill_exp;
}

void OutputSectDesc::activate()
{
  for (const_iterator it = begin(), ie = end(); it != ie; ++it) {
    switch ((*it)->getKind()) {
    case ScriptCommand::Assignment:
    case ScriptCommand::InputSectDesc:
      (*it)->activate();
      break;
    default:
      assert(0);
      break;
    }
  }
}
