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

/// initMCLinker - initialize MCLinker
///  Connect all components in MCLinker
bool MCLDDriver::initMCLinker()
{
  return true;
}

/// readSections - read all input section headers
bool MCLDDriver::readSections()
{
  return true;
}

/// mergeSections - put allinput sections into output sections
bool MCLDDriver::mergeSections()
{
  return true;
}

/// readSymbolTables - read symbol tables from the input files.
///  for each input file, loads its symbol table from file.
bool MCLDDriver::readSymbolTables()
{
  return true;
}

/// mergeSymbolTables - merge the symbol tables of input files into the
/// output's symbol table.
bool MCLDDriver::mergeSymbolTables()
{
  return true;
}

/// addStandardSymbols - shared object and executable files need some
/// standard symbols
///   @return if there are some input symbols with the same name to the
///   standard symbols, return false
bool MCLDDriver::addStandardSymbols()
{
  return true;
}

/// addTargetSymbols - some targets, such as MIPS and ARM, need some
/// target-dependent symbols
///   @return if there are some input symbols with the same name to the
///   target symbols, return false
bool MCLDDriver::addTargetSymbols()
{
  return true;
}

/// readRelocations - read all relocation entries
bool MCLDDriver::readRelocations()
{
  return true;
}

/// layout - linearly layout all output sections and reserve some space
/// for GOT/PLT
///   Because we do not support instruction relaxing in this early version,
///   if there is a branch can not jump to its target, we return false
///   directly
bool MCLDDriver::layout()
{
  return true;
}

/// relocate - applying relocation entries and create relocation
/// section in the output files
/// Create relocation section, asking TargetLDBackend to
/// read the relocation information into RelocationEntry
/// and push_back into the relocation section
bool MCLDDriver::relocate()
{
  return true;
}

/// emitOutput - emit the output file.
bool MCLDDriver::emitOutput()
{
  return true;
}

} // end namespace mcld
