//===- ObjectLinker.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Object/ObjectLinker.h>
#include <mcld/LinkerConfig.h>
#include <mcld/Module.h>
#include <mcld/MC/InputTree.h>
#include <mcld/MC/InputBuilder.h>
#include <mcld/LD/Archive.h>
#include <mcld/LD/ArchiveReader.h>
#include <mcld/LD/ObjectReader.h>
#include <mcld/LD/DynObjReader.h>
#include <mcld/LD/GroupReader.h>
#include <mcld/LD/ObjectWriter.h>
#include <mcld/LD/DynObjWriter.h>
#include <mcld/LD/ExecWriter.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/Support/RealPath.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/Fragment/FragmentLinker.h>

using namespace llvm;
using namespace mcld;

ObjectLinker::ObjectLinker(const LinkerConfig& pConfig,
                           Module& pModule,
                           InputBuilder& pBuilder,
                           TargetLDBackend& pLDBackend)
  : m_Config(pConfig),
    m_Module(pModule),
    m_Builder(pBuilder),
    m_pLinker(NULL),
    m_LDBackend(pLDBackend),
    m_pObjectReader(NULL),
    m_pDynObjReader(NULL),
    m_pArchiveReader(NULL),
    m_pObjectWriter(NULL),
    m_pDynObjWriter(NULL),
    m_pExecWriter(NULL),
    m_pGroupReader(NULL)
{
}

ObjectLinker::~ObjectLinker()
{
  delete m_pLinker;
  if (NULL != m_pObjectReader)
    delete m_pObjectReader;
  if (NULL != m_pDynObjReader)
    delete m_pDynObjReader;
  if (NULL != m_pArchiveReader)
    delete m_pArchiveReader;
  if (NULL != m_pObjectWriter)
    delete m_pObjectWriter;
  if (NULL != m_pDynObjWriter)
    delete m_pDynObjWriter;
  if (NULL != m_pExecWriter)
    delete m_pExecWriter;
  if (NULL != m_pGroupReader)
    delete m_pGroupReader;
}

/// initFragmentLinker - initialize FragmentLinker
///  Connect all components with FragmentLinker
bool ObjectLinker::initFragmentLinker()
{
  if (NULL == m_pLinker)
    m_pLinker = new FragmentLinker(m_Config,
                                   m_Module,
                                   m_LDBackend,
                                   m_SectionMap);

  // initialize the readers and writers
  // Because constructor can not be failed, we initalize all readers and
  // writers outside the FragmentLinker constructors.
  m_pObjectReader  = m_LDBackend.createObjectReader(*m_pLinker);
  m_pArchiveReader = m_LDBackend.createArchiveReader(m_Module);
  m_pDynObjReader  = m_LDBackend.createDynObjReader(*m_pLinker);
  m_pObjectWriter  = m_LDBackend.createObjectWriter(*m_pLinker);
  m_pDynObjWriter  = m_LDBackend.createDynObjWriter(*m_pLinker);
  m_pExecWriter    = m_LDBackend.createExecWriter(*m_pLinker);
  m_pGroupReader   = new GroupReader(m_Module, *m_pObjectReader,
                                     *m_pDynObjReader, *m_pArchiveReader);

  // initialize RelocationFactory
  m_LDBackend.initRelocFactory(*m_pLinker);
  return true;
}

/// initStdSections - initialize standard sections
bool ObjectLinker::initStdSections()
{
  /// initialize section mapping for standard format, target-dependent section,
  /// (and user-defined mapping)
  if (!m_SectionMap.initStdSectionMap() ||
      !m_LDBackend.initTargetSectionMap(m_SectionMap))
    return false;

  /// A technical debt. We need to initialize section map here because
  /// we do not separate output file and temporary data structure. So far,
  /// FragmentLinker directly use output file's LDContext as the temporary data
  /// structure. We will create a new data structure mcld::Module to collect
  /// all temporary data structures togather.
  m_pLinker->initSectionMap();

  // initialize standard sections
  switch (m_Config.codeGenType()) {
    case LinkerConfig::DynObj: {
      // intialize standard and target-dependent sections
      if (!m_LDBackend.initDynObjSections(*m_pLinker))
        return false;
      break;
    }
    case LinkerConfig::Exec: {
      // intialize standard and target-dependent sections
      if (!m_LDBackend.initExecSections(*m_pLinker))
        return false;
      break;
    }
    case LinkerConfig::Object: {
      llvm::report_fatal_error(llvm::Twine("output type is not implemented yet. file: `") +
                               m_Module.name() +
                               llvm::Twine("'."));
      return false;
    }
    default: {
      llvm::report_fatal_error(llvm::Twine("unknown output type of file `") +
                               m_Module.name() +
                               llvm::Twine("'."));
       return false;
    }
  } // end of switch

  // initialize target-dependent sections
  m_LDBackend.initTargetSections(m_Module, *m_pLinker);

  return true;
}

void ObjectLinker::normalize()
{
  // -----  set up inputs  ----- //
  Module::input_iterator input, inEnd = m_Module.input_end();
  for (input = m_Module.input_begin(); input!=inEnd; ++input) {
    // is a group node
    if (isGroup(input)) {
      getGroupReader()->readGroup(input, m_Builder, m_Config);
      continue;
    }

    // already got type - for example, bitcode or external OIR (object
    // intermediate representation)
    if ((*input)->type() == Input::Script ||
        (*input)->type() == Input::Object ||
        (*input)->type() == Input::DynObj  ||
        (*input)->type() == Input::Archive ||
        (*input)->type() == Input::External)
      continue;

    // is a relocatable object file
    if (getObjectReader()->isMyFormat(**input)) {
      (*input)->setType(Input::Object);
      getObjectReader()->readHeader(**input);
      getObjectReader()->readSections(**input);
      getObjectReader()->readSymbols(**input);
      m_Module.getObjectList().push_back(*input);
    }
    // is a shared object file
    else if (getDynObjReader()->isMyFormat(**input)) {
      (*input)->setType(Input::DynObj);
      getDynObjReader()->readHeader(**input);
      getDynObjReader()->readSymbols(**input);
      m_Module.getLibraryList().push_back(*input);
    }
    // is an archive
    else if (getArchiveReader()->isMyFormat(**input)) {
      (*input)->setType(Input::Archive);
      Archive archive(**input, m_Builder);
      getArchiveReader()->readArchive(archive);
      if(archive.numOfObjectMember() > 0) {
        m_Module.getInputTree().merge<InputTree::Inclusive>(input,
                                                            archive.inputs());
      }
    }
    else {
      fatal(diag::err_unrecognized_input_file) << (*input)->path()
                                               << m_Config.triple().str();
    }
  } // end of for
}

bool ObjectLinker::linkable() const
{
  // check we have input and output files
  if (m_Module.getInputTree().empty()) {
    error(diag::err_no_inputs);
    return false;
  }

  // can not mix -static with shared objects
  Module::const_lib_iterator lib, libEnd = m_Module.lib_end();
  for (lib = m_Module.lib_begin(); lib != libEnd; ++lib) {
    if((*lib)->attribute()->isStatic()) {
      error(diag::err_mixed_shared_static_objects)
                                      << (*lib)->name() << (*lib)->path();
      return false;
    }
  }

  // can not mix -r with shared objects
  return true;
}

/// mergeSections - put allinput sections into output sections
bool ObjectLinker::mergeSections()
{
  // TODO: when FragmentLinker can read other object files, we have to merge
  // sections
  return true;
}

/// addStandardSymbols - shared object and executable files need some
/// standard symbols
///   @return if there are some input symbols with the same name to the
///   standard symbols, return false
bool ObjectLinker::addStandardSymbols()
{
  return m_LDBackend.initStandardSymbols(*m_pLinker);
}

/// addTargetSymbols - some targets, such as MIPS and ARM, need some
/// target-dependent symbols
///   @return if there are some input symbols with the same name to the
///   target symbols, return false
bool ObjectLinker::addTargetSymbols()
{
  m_LDBackend.initTargetSymbols(*m_pLinker);
  return true;
}

/// readRelocations - read all relocation entries
///
/// All symbols should be read and resolved before this function.
bool ObjectLinker::readRelocations()
{
  // Bitcode is read by the other path. This function reads relocation sections
  // in object files.
  mcld::InputTree::bfs_iterator input, inEnd = m_Module.getInputTree().bfs_end();
  for (input=m_Module.getInputTree().bfs_begin(); input!=inEnd; ++input) {
    if ((*input)->type() == Input::Object) {
      if (!getObjectReader()->readRelocations(**input))
        return false;
    }
    // ignore the other kinds of files.
  }
  return true;
}

/// prelayout - help backend to do some modification before layout
bool ObjectLinker::prelayout()
{
  m_LDBackend.preLayout(m_Module, *m_pLinker);

  m_LDBackend.allocateCommonSymbols(m_Module, *m_pLinker);

  /// check program interpreter - computer the name size of the runtime dyld
  m_LDBackend.sizeInterp();

  /// measure NamePools - compute the size of name pool sections
  /// In ELF, will compute  the size of.symtab, .strtab, .dynsym, .dynstr,
  /// and .hash sections.
  ///
  /// dump all symbols and strings from FragmentLinker and build the format-dependent
  /// hash table.
  m_LDBackend.sizeNamePools(m_Module);

  return true;
}

/// layout - linearly layout all output sections and reserve some space
/// for GOT/PLT
///   Because we do not support instruction relaxing in this early version,
///   if there is a branch can not jump to its target, we return false
///   directly
bool ObjectLinker::layout()
{
  return m_pLinker->layout();
}

/// prelayout - help backend to do some modification after layout
bool ObjectLinker::postlayout()
{
  m_LDBackend.postLayout(m_Module, *m_pLinker);
  return true;
}

/// finalizeSymbolValue - finalize the resolved symbol value.
///   Before relocate(), after layout(), FragmentLinker should correct value of all
///   symbol.
bool ObjectLinker::finalizeSymbolValue()
{
  return m_pLinker->finalizeSymbols();
}

/// relocate - applying relocation entries and create relocation
/// section in the output files
/// Create relocation section, asking TargetLDBackend to
/// read the relocation information into RelocationEntry
/// and push_back into the relocation section
bool ObjectLinker::relocation()
{
  return m_pLinker->applyRelocations();
}

/// emitOutput - emit the output file.
bool ObjectLinker::emitOutput(MemoryArea& pOutput)
{
  switch(m_Config.codeGenType()) {
    case LinkerConfig::Object:
      getObjectWriter()->writeObject(m_Module, pOutput);
      return true;
    case LinkerConfig::DynObj:
      getDynObjWriter()->writeDynObj(m_Module, pOutput);
      return true;
    case LinkerConfig::Exec:
      getExecWriter()->writeExecutable(m_Module, pOutput);
      return true;
  }
  return false;
}

/// postProcessing - do modification after all processes
bool ObjectLinker::postProcessing(MemoryArea& pOutput)
{
  m_pLinker->syncRelocationResult(pOutput);

  // emit .eh_frame_hdr
  // eh_frame_hdr should be emitted after syncRelocation, because eh_frame_hdr
  // needs FDE PC value, which will be corrected at syncRelocation
  m_LDBackend.postProcessing(*m_pLinker, pOutput);
  return true;
}

