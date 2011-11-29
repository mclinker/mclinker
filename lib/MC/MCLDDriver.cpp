//===- MCLDDriver.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "mcld/MC/MCLDDriver.h"

namespace mcld {

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
        (*input)->setMemArea(m_LDInfo.memAreaFactory().produce((*input)->path(), O_RDONLY));
        if (!(*input)->memArea()->isGood())
          report_fatal_error("can not open file: " + (*input)->path().native());
        break;
      default:
        report_fatal_error("can not link file: " + (*input)->path().native());
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
    if (!m_LDInfo.attrFactory().constraint().isLegal((**attr), error_code)) {
      report_fatal_error(error_code);
      return false;
    }
  }


  bool hasDynObj = false;
  // can not mix -static with shared objects
  mcld::InputTree::const_bfs_iterator input, inEnd = m_LDInfo.inputs().bfs_end();
  for (input=m_LDInfo.inputs().bfs_begin(); input!=inEnd; ++input) {
    if ((*input)->type() == mcld::Input::DynObj ) {
      hasDynObj = true;
      if((*input)->attribute()->isStatic()) {
        report_fatal_error("Can't link shared object with -static option");
        return false;
      }
    }
  }

  // can not mix -r with shared objects
  return true;
}

void MCLDDriver::readSymbolTables()
{
}

void MCLDDriver::mergeSymbolTables()
{
}

} // end namespace mcld
