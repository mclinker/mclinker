/*****************************************************************************
 *   The mcld Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@mediatek.com>                                         *
 *   Luba Tang <luba.tang@mediatek.com>                                      *
 ****************************************************************************/
#include <llvm/Support/ErrorHandling.h>
#include <mcld/MC/MCLDDriver.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/MC/MCLDInfo.h>
#include <string>

using namespace mcld;

MCLDDriver::MCLDDriver(MCLDInfo& pLDInfo, TargetLDBackend& pLDBackend)
  : m_LDInfo(pLDInfo), m_LDBackend(pLDBackend) {
}

MCLDDriver::~MCLDDriver()
{
}

bool MCLDDriver::linkable() const
{
  // check all attributes are legal
  mcld::AttributeFactory::const_iterator attr, attrEnd = m_LDInfo.attrFactory().end();
  for (attr=m_LDInfo.attrFactory().begin(); attr!=attrEnd; ++attr) {
    std::string error_code;
    if (!m_LDInfo.attrFactory().constraint().isLegal((**attr), error_code))
      report_fatal_error(error_code);
  }

  // after normalization, all input files are regnized.
  mcld::InputTree::const_bfs_iterator input, inEnd = m_LDInfo.inputs().bfs_end();
  for (input=m_LDInfo.inputs().bfs_begin(); input!=inEnd; ++input) {
    if ((*input)->type() == mcld::Input::DynObj && (*input)->attribute()->isStatic())
      report_fatal_error("Can't link shared object with -static option");
  }
}

