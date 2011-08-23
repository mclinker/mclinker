/*****************************************************************************
 *   The mcld Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@mediatek.com>                                         *
 *   Luba Tang <luba.tang@mediatek.com>                                      *
 ****************************************************************************/
#include <llvm/Support/ErrorHandling.h>

#include <mcld/MC/MCLDInputTree.h>
#include <mcld/MC/MCObjectReader.h>
#include <mcld/MC/MCArchiveReader.h>
#include <mcld/MC/MCLDDriver.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/Support/RealPath.h>
#include <mcld/Target/TargetLDBackend.h>

#ifdef MCLD_DEBUG
#include <iostream>
using namespace std;
#endif

using namespace mcld;

MCLDDriver::MCLDDriver(MCLDInfo& pLDInfo, TargetLDBackend& pLDBackend)
  : m_LDInfo(pLDInfo), m_LDBackend(pLDBackend) {
}

MCLDDriver::~MCLDDriver()
{
}

void MCLDDriver::normalize() {

  InputTree::dfs_iterator input, inEnd = m_LDInfo.inputs().dfs_end();
  Input::Type type;
  for (input = m_LDInfo.inputs().dfs_begin(); input!=inEnd; ++input) {
    if ((*input)->type() == Input::Object)
      continue;
    //ObjectFile or Dynamic Object
    else if (m_LDBackend.getObjectReader()->isMyFormat(*(*input))) {
      switch (type = m_LDBackend.getObjectReader()->fileType(*(*input))) {
      case Input::DynObj:
      case Input::Object:
        (*input)->setType(type);
        (*input)->setContext(m_LDInfo.contextFactory().produce((*input)->path()));
        break;
      default:
        report_fatal_error("can not link file: " + (*input)->path().string());
        break;
      }
    }
    else if (m_LDBackend.getArchiveReader()->isMyFormat(*(*input))) {
      (*input)->setType(Input::Archive);
      mcld::InputTree* archive_member = m_LDBackend.getArchiveReader()->readArchive(**input);
      if(!archive_member) 
        report_fatal_error("wrong format archive" + (*input)->path().string());

      m_LDInfo.inputs().merge<InputTree::Inclusive>(input, *archive_member);
    }
  }
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

  // after normalization, all input files are recognized.
  mcld::InputTree::const_bfs_iterator input, inEnd = m_LDInfo.inputs().bfs_end();
  for (input=m_LDInfo.inputs().bfs_begin(); input!=inEnd; ++input) {
    if ((*input)->type() == mcld::Input::DynObj && (*input)->attribute()->isStatic())
      report_fatal_error("Can't link shared object with -static option");
  }
}

