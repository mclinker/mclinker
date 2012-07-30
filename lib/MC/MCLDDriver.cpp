//===- MCLDDriver.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/MC/InputTree.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/MC/MCLDDriver.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/LD/ArchiveReader.h>
#include <mcld/LD/ObjectReader.h>
#include <mcld/LD/DynObjReader.h>
#include <mcld/LD/ObjectWriter.h>
#include <mcld/LD/DynObjWriter.h>
#include <mcld/LD/ExecWriter.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/Support/RealPath.h>
#include <mcld/Support/MemoryAreaFactory.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/LD/Archive.h>

using namespace llvm;
using namespace mcld;

MCLDDriver::MCLDDriver(MCLDInfo& pLDInfo,
                       TargetLDBackend& pLDBackend,
                       MemoryAreaFactory& pAreaFactory)
  : m_LDInfo(pLDInfo),
    m_LDBackend(pLDBackend),
    m_pLinker(NULL),
    m_AreaFactory(pAreaFactory) {

}

MCLDDriver::~MCLDDriver()
{
  if (NULL != m_pLinker)
    delete m_pLinker;
}

/// initMCLinker - initialize MCLinker
///  Connect all components with MCLinker
bool MCLDDriver::initMCLinker()
{
  if (0 == m_pLinker)
    m_pLinker = new MCLinker(m_LDBackend,
                             m_LDInfo,
                             m_SectionMap);

  // initialize the readers and writers
  // Because constructor can not be failed, we initalize all readers and
  // writers outside the MCLinker constructors.
  if (!m_LDBackend.initObjectReader(*m_pLinker) ||
      !m_LDBackend.initArchiveReader(*m_pLinker, m_LDInfo, m_AreaFactory) ||
      !m_LDBackend.initObjectReader(*m_pLinker) ||
      !m_LDBackend.initDynObjReader(*m_pLinker) ||
      !m_LDBackend.initObjectWriter(*m_pLinker) ||
      !m_LDBackend.initDynObjWriter(*m_pLinker) ||
      !m_LDBackend.initExecWriter(*m_pLinker))
    return false;

  // initialize RelocationFactory
  m_LDBackend.initRelocFactory(*m_pLinker);
  return true;
}

/// initStdSections - initialize standard sections
bool MCLDDriver::initStdSections()
{
  /// initialize section mapping for standard format, target-dependent section,
  /// (and user-defined mapping)
  if (!m_SectionMap.initStdSectionMap() ||
      !m_LDBackend.initTargetSectionMap(m_SectionMap))
    return false;

  /// A technical debt. We need to initialize section map here because
  /// we do not separate output file and temporary data structure. So far,
  /// MCLinker directly use output file's LDContext as the temporary data
  /// structure. We will create a new data structure mcld::Module to collect
  /// all temporary data structures togather.
  m_pLinker->initSectionMap();

  // initialize standard sections
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

  // initialize target-dependent sections
  m_LDBackend.initTargetSections(*m_pLinker);

  return true;
}

void MCLDDriver::normalize()
{
  // -----  set up inputs  ----- //
  InputTree::iterator input, inEnd = m_LDInfo.inputs().end();
  for (input = m_LDInfo.inputs().begin(); input!=inEnd; ++input) {
    // already got type - for example, bitcode or external OIR (object
    // intermediate representation)
    if ((*input)->type() == Input::Script ||
        (*input)->type() == Input::Object ||
        (*input)->type() == Input::DynObj  ||
        (*input)->type() == Input::Archive ||
        (*input)->type() == Input::External)
      continue;

    // is a relocatable object file
    if (m_LDBackend.getObjectReader()->isMyFormat(**input)) {
      (*input)->setType(Input::Object);
      m_LDBackend.getObjectReader()->readObject(**input);
      m_LDBackend.getObjectReader()->readSections(**input);
      m_LDBackend.getObjectReader()->readSymbols(**input);
    }
    // is a shared object file
    else if (m_LDBackend.getDynObjReader()->isMyFormat(**input)) {
      (*input)->setType(Input::DynObj);
      m_LDBackend.getDynObjReader()->readDSO(**input);
      m_LDBackend.getDynObjReader()->readSymbols(**input);
    }
    // is an archive
    else if (m_LDBackend.getArchiveReader()->isMyFormat(**input)) {
      (*input)->setType(Input::Archive);
      Archive archive(**input, m_LDInfo.inputFactory());
      m_LDBackend.getArchiveReader()->readArchive(archive);
      if(archive.numOfObjectMember() > 0) {
        m_LDInfo.inputs().merge<InputTree::Inclusive>(input, archive.inputs());
      }
    }
    else {
      fatal(diag::err_unrecognized_input_file) << (*input)->path()
                                               << m_LDInfo.triple().str();
    }
  } // end of for
}

bool MCLDDriver::linkable() const
{
  // check we have input and output files
  if (m_LDInfo.inputs().empty()) {
    error(diag::err_no_inputs);
    return false;
  }

  // check all attributes are legal
  mcld::AttributeFactory::const_iterator attr, attrEnd = m_LDInfo.attrFactory().end();
  for (attr=m_LDInfo.attrFactory().begin(); attr!=attrEnd; ++attr) {
    if (!m_LDInfo.attrFactory().constraint().isLegal((**attr))) {
      return false;
    }
  }

  // can not mix -static with shared objects
  mcld::InputTree::const_bfs_iterator input, inEnd = m_LDInfo.inputs().bfs_end();
  for (input=m_LDInfo.inputs().bfs_begin(); input!=inEnd; ++input) {
    if ((*input)->type() == mcld::Input::DynObj) {
      if((*input)->attribute()->isStatic()) {
        error(diag::err_mixed_shared_static_objects)
                                        << (*input)->name() << (*input)->path();
        return false;
      }
    }
  }

  // can not mix -r with shared objects
  return true;
}

/// mergeSections - put allinput sections into output sections
bool MCLDDriver::mergeSections()
{
  // TODO: when MCLinker can read other object files, we have to merge
  // sections
  return true;
}

/// addStandardSymbols - shared object and executable files need some
/// standard symbols
///   @return if there are some input symbols with the same name to the
///   standard symbols, return false
bool MCLDDriver::addStandardSymbols()
{
  return m_LDBackend.initStandardSymbols(*m_pLinker, m_LDInfo.output());
}

/// addTargetSymbols - some targets, such as MIPS and ARM, need some
/// target-dependent symbols
///   @return if there are some input symbols with the same name to the
///   target symbols, return false
bool MCLDDriver::addTargetSymbols()
{
  m_LDBackend.initTargetSymbols(*m_pLinker, m_LDInfo.output());
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

  /// check program interpreter - computer the name size of the runtime dyld
  /// FIXME: check if we are doing static linking!
  if (m_LDInfo.output().type() == Output::Exec)
    m_LDBackend.sizeInterp(m_LDInfo.output(), m_LDInfo);

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

/// finalizeSymbolValue - finalize the resolved symbol value.
///   Before relocate(), after layout(), MCLinker should correct value of all
///   symbol.
bool MCLDDriver::finalizeSymbolValue()
{
  return m_pLinker->finalizeSymbols();
}

/// relocate - applying relocation entries and create relocation
/// section in the output files
/// Create relocation section, asking TargetLDBackend to
/// read the relocation information into RelocationEntry
/// and push_back into the relocation section
bool MCLDDriver::relocation()
{
  return m_pLinker->applyRelocations();
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
    case Output::Exec:
      m_LDBackend.getExecWriter()->writeExecutable(m_LDInfo.output());
      return true;
  }
  return false;
}

/// postProcessing - do modification after all processes
bool MCLDDriver::postProcessing()
{
  m_pLinker->syncRelocationResult();

  m_LDBackend.postProcessing(m_LDInfo.output(),
                             m_LDInfo,
                             *m_pLinker);
  return true;
}
