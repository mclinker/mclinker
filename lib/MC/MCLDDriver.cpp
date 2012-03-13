//===- MCLDDriver.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/MC/MCLinker.h>
#include <mcld/MC/MCLDInputTree.h>
#include <mcld/MC/MCLDDriver.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/LD/ArchiveReader.h>
#include <mcld/LD/ObjectReader.h>
#include <mcld/LD/DynObjReader.h>
#include <mcld/LD/ObjectWriter.h>
#include <mcld/LD/DynObjWriter.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/Support/RealPath.h>
#include <mcld/Target/TargetLDBackend.h>
#include <llvm/Support/ErrorHandling.h>

using namespace llvm;
using namespace mcld;

MCLDDriver::MCLDDriver(MCLDInfo& pLDInfo, TargetLDBackend& pLDBackend)
  : m_LDInfo(pLDInfo),
    m_LDBackend(pLDBackend),
    m_pLinker(0) {
}

MCLDDriver::~MCLDDriver()
{
  if (0 != m_pLinker)
    delete m_pLinker;
}

void MCLDDriver::normalize() {

  InputTree::dfs_iterator input, inEnd = m_LDInfo.inputs().dfs_end();
  for (input = m_LDInfo.inputs().dfs_begin(); input!=inEnd; ++input) {
    // already got type - for example, bitcode
    if ((*input)->type() == Input::Script ||
        (*input)->type() == Input::Object ||
        (*input)->type() == Input::DynObj  ||
        (*input)->type() == Input::Archive)
      continue;


    MemoryArea *input_memory =
        m_LDInfo.memAreaFactory().produce((*input)->path(), O_RDONLY);
    if ((input_memory != NULL) && input_memory->isGood()) {
      (*input)->setMemArea(input_memory);
    }
    else {
      llvm::report_fatal_error("can not open file: " + (*input)->path().native());
      return;
    }

    // is a relocatable object file
    if (m_LDBackend.getObjectReader()->isMyFormat(**input)) {
      (*input)->setType(Input::Object);
      (*input)->setContext(m_LDInfo.contextFactory().produce((*input)->path()));
      m_LDBackend.getObjectReader()->readObject(**input);
    }
    // is a shared object file
    else if (m_LDBackend.getDynObjReader()->isMyFormat(**input)) {
      (*input)->setType(Input::DynObj);
      (*input)->setContext(m_LDInfo.contextFactory().produce((*input)->path()));
      (*input)->setSOName((*input)->path().native());
      m_LDBackend.getDynObjReader()->readDSO(**input);
    }
    // is an archive
    else if (m_LDBackend.getArchiveReader()->isMyFormat(*(*input))) {
      (*input)->setType(Input::Archive);
      mcld::InputTree* archive_member = m_LDBackend.getArchiveReader()->readArchive(**input);
      if(!archive_member)  {
        llvm::report_fatal_error("wrong format archive" + (*input)->path().string());
        return;
      }

      m_LDInfo.inputs().merge<InputTree::Inclusive>(input, *archive_member);
    }
    else {
      llvm::report_fatal_error(llvm::Twine("can not recognize file format: ") +
                               (*input)->path().native() +
                               llvm::Twine("\nobject format or target machine is wrong\n"));
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
///  Connect all components with MCLinker
bool MCLDDriver::initMCLinker()
{
  if (0 == m_pLinker)
    m_pLinker = new MCLinker(m_LDBackend,
                             m_LDInfo,
                             *m_LDInfo.output().context(),
                             m_SectionMap);

  // initialize the readers and writers
  // Because constructor can not be failed, we initalize all readers and
  // writers outside the MCLinker constructors.
  if (!m_LDBackend.initArchiveReader(*m_pLinker, m_LDInfo) ||
      !m_LDBackend.initObjectReader(*m_pLinker) ||
      !m_LDBackend.initDynObjReader(*m_pLinker) ||
      !m_LDBackend.initObjectWriter(*m_pLinker) ||
      !m_LDBackend.initDynObjWriter(*m_pLinker))
    return false;

  /// initialize section mapping for standard format, target-dependent section,
  /// (and user-defined mapping)
  if (!m_SectionMap.initStdSectionMap() ||
      !m_LDBackend.initTargetSectionMap(m_SectionMap))
    return false;

  // initialize standard segments and sections
  switch (m_LDInfo.output().type()) {
    case Output::DynObj: {
      // intialize standard and target-dependent sections
      if (!m_LDBackend.initDynObjSections(*m_pLinker))
        return false;
      break;
    }
    case Output::Exec: {
      // intialize standard and target-dependent sections
      if (!m_LDBackend.initExecSections(*m_pLinker))
        return false;
      break;
    }
    case Output::Object: {
      llvm::report_fatal_error(llvm::Twine("output type is not implemented yet. file: `") +
                               m_LDInfo.output().name() +
                               llvm::Twine("'."));
      return false;
    }
    default: {
      llvm::report_fatal_error(llvm::Twine("unknown output type of file `") +
                               m_LDInfo.output().name() +
                               llvm::Twine("'."));
       return false;
    }
  } // end of switch

  // initialize target-dependent segments and sections
  m_LDBackend.initTargetSections(*m_pLinker);

  // initialize RelocationFactory
  m_LDBackend.initRelocFactory(*m_pLinker);

  return true;
}

/// readSections - read all input section headers
bool MCLDDriver::readSections()
{
  // Bitcode is read by the other path. This function reads sections in object
  // files.
  mcld::InputTree::bfs_iterator input, inEnd = m_LDInfo.inputs().bfs_end();
  for (input=m_LDInfo.inputs().bfs_begin(); input!=inEnd; ++input) {
    if ((*input)->type() == Input::Object) {
      if (!m_LDBackend.getObjectReader()->readSections(**input))
        return false;
    }
  }
  return true;
}

/// mergeSections - put allinput sections into output sections
bool MCLDDriver::mergeSections()
{
  // TODO: when MCLinker can read other object files, we have to merge
  // sections
  return true;
}

/// readSymbolTables - read symbol tables from the input files.
///  for each input file, loads its symbol table from file.
bool MCLDDriver::readSymbolTables()
{
  mcld::InputTree::dfs_iterator input, inEnd = m_LDInfo.inputs().dfs_end();
  for (input=m_LDInfo.inputs().dfs_begin(); input!=inEnd; ++input) {
    switch((*input)->type()) {
    case Input::DynObj:
      if (!m_LDBackend.getDynObjReader()->readSymbols(**input))
        return false;
      break;
    case Input::Object:
      if (!m_LDBackend.getObjectReader()->readSymbols(**input))
        return false;
      break;
    }
  }
  return true;
}

/// mergeSymbolTables - merge the symbol tables of input files into the
/// output's symbol table.
bool MCLDDriver::mergeSymbolTables()
{
  mcld::InputTree::dfs_iterator input, inEnd = m_LDInfo.inputs().dfs_end();
  for (input=m_LDInfo.inputs().dfs_begin(); input!=inEnd; ++input) {
    if (!m_pLinker->mergeSymbolTable(**input))
      return false;
  }
  return true;
}

/// addStandardSymbols - shared object and executable files need some
/// standard symbols
///   @return if there are some input symbols with the same name to the
///   standard symbols, return false
bool MCLDDriver::addStandardSymbols()
{
  return m_LDBackend.initStandardSymbols(*m_pLinker);
}

/// addTargetSymbols - some targets, such as MIPS and ARM, need some
/// target-dependent symbols
///   @return if there are some input symbols with the same name to the
///   target symbols, return false
bool MCLDDriver::addTargetSymbols()
{
  m_LDBackend.initTargetSymbols(*m_pLinker);
  return true;
}

/// readRelocations - read all relocation entries
///
/// All symbols should be read and resolved before this function.
bool MCLDDriver::readRelocations()
{
  // Bitcode is read by the other path. This function reads relocation sections
  // in object files.
  mcld::InputTree::bfs_iterator input, inEnd = m_LDInfo.inputs().bfs_end();
  for (input=m_LDInfo.inputs().bfs_begin(); input!=inEnd; ++input) {
    if ((*input)->type() == Input::Object) {
      if (!m_LDBackend.getObjectReader()->readRelocations(**input))
        return false;
    }
    // ignore the other kinds of files.
  }
  return true;
}

/// prelayout - help backend to do some modification before layout
bool MCLDDriver::prelayout()
{
  m_LDBackend.preLayout(m_LDInfo.output(),
                        m_LDInfo,
                        *m_pLinker);

  m_LDBackend.allocateCommonSymbols(m_LDInfo, *m_pLinker);

  /// measure NamePools - compute the size of name pool sections
  /// In ELF, will compute  the size of.symtab, .strtab, .dynsym, .dynstr,
  /// and .hash sections.
  ///
  /// dump all symbols and strings from MCLinker and build the format-dependent
  /// hash table.
  m_LDBackend.sizeNamePools(m_LDInfo.output(), m_pLinker->getOutputSymbols(), m_LDInfo);

  return true;
}

/// layout - linearly layout all output sections and reserve some space
/// for GOT/PLT
///   Because we do not support instruction relaxing in this early version,
///   if there is a branch can not jump to its target, we return false
///   directly
bool MCLDDriver::layout()
{
  return m_pLinker->layout();
}

/// prelayout - help backend to do some modification after layout
bool MCLDDriver::postlayout()
{
  m_LDBackend.postLayout(m_LDInfo.output(),
                         m_LDInfo,
                         *m_pLinker);
  return true;
}

/// relocate - applying relocation entries and create relocation
/// section in the output files
/// Create relocation section, asking TargetLDBackend to
/// read the relocation information into RelocationEntry
/// and push_back into the relocation section
bool MCLDDriver::relocate()
{
  return m_pLinker->applyRelocations();
}

/// finalizeSymbolValue - finalize the resolved symbol value.
///   Before relocate(), after layout(), MCLinker should correct value of all
///   symbol.
bool MCLDDriver::finalizeSymbolValue()
{
  return m_pLinker->finalizeSymbols();
}

/// emitOutput - emit the output file.
bool MCLDDriver::emitOutput()
{
  switch(m_LDInfo.output().type()) {
    case Output::Object:
      m_LDBackend.getObjectWriter()->writeObject(m_LDInfo.output());
      return true;
    case Output::DynObj:
      m_LDBackend.getDynObjWriter()->writeDynObj(m_LDInfo.output());
      return true;
    /** TODO: open the executable file writer **/
    // case Output::Exec:
      // m_LDBackend.getExecWriter()->writeObject(m_LDInfo.output());
      // return true;
  }
  return false;
}

/// postProcessing - do modification after all processes
bool MCLDDriver::postProcessing()
{
  m_pLinker->syncRelocationResult();
  return true;
}
