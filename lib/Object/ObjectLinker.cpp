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
#include <mcld/InputTree.h>
#include <mcld/IRBuilder.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/LDContext.h>
#include <mcld/LD/Archive.h>
#include <mcld/LD/ArchiveReader.h>
#include <mcld/LD/ObjectReader.h>
#include <mcld/LD/DynObjReader.h>
#include <mcld/LD/GroupReader.h>
#include <mcld/LD/BinaryReader.h>
#include <mcld/LD/ObjectWriter.h>
#include <mcld/LD/DynObjWriter.h>
#include <mcld/LD/ExecWriter.h>
#include <mcld/LD/BinaryWriter.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/RelocData.h>
#include <mcld/Support/RealPath.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/Fragment/FragmentLinker.h>
#include <mcld/Object/ObjectBuilder.h>

#include <llvm/Support/Casting.h>

using namespace llvm;
using namespace mcld;

ObjectLinker::ObjectLinker(const LinkerConfig& pConfig,
                           Module& pModule,
                           IRBuilder& pBuilder,
                           TargetLDBackend& pLDBackend)
  : m_Config(pConfig),
    m_Module(pModule),
    m_Builder(pBuilder),
    m_pLinker(NULL),
    m_LDBackend(pLDBackend),
    m_pObjectReader(NULL),
    m_pDynObjReader(NULL),
    m_pArchiveReader(NULL),
    m_pGroupReader(NULL),
    m_pBinaryReader(NULL),
    m_pObjectWriter(NULL),
    m_pDynObjWriter(NULL),
    m_pExecWriter(NULL),
    m_pBinaryWriter(NULL)
{
  // set up soname
  if (!m_Config.options().soname().empty()) {
    m_Module.setName(m_Config.options().soname());
  }
}

ObjectLinker::~ObjectLinker()
{
  delete m_pLinker;
  delete m_pObjectReader;
  delete m_pDynObjReader;
  delete m_pArchiveReader;
  delete m_pGroupReader;
  delete m_pBinaryReader;
  delete m_pObjectWriter;
  delete m_pDynObjWriter;
  delete m_pExecWriter;
  delete m_pBinaryWriter;
}

/// initFragmentLinker - initialize FragmentLinker
///  Connect all components with FragmentLinker
bool ObjectLinker::initFragmentLinker()
{
  if (NULL == m_pLinker) {
    m_pLinker = new FragmentLinker(m_Config,
                                   m_Module,
                                   m_LDBackend);
  }

  // initialize the readers and writers
  // Because constructor can not be failed, we initalize all readers and
  // writers outside the FragmentLinker constructors.
  m_pObjectReader  = m_LDBackend.createObjectReader(m_Builder);
  m_pArchiveReader = m_LDBackend.createArchiveReader(m_Module);
  m_pDynObjReader  = m_LDBackend.createDynObjReader(m_Builder);
  m_pGroupReader   = new GroupReader(m_Module, *m_pObjectReader,
                                     *m_pDynObjReader, *m_pArchiveReader);
  m_pBinaryReader  = m_LDBackend.createBinaryReader(m_Builder);
  m_pObjectWriter  = m_LDBackend.createObjectWriter();
  m_pDynObjWriter  = m_LDBackend.createDynObjWriter();
  m_pExecWriter    = m_LDBackend.createExecWriter();
  m_pBinaryWriter  = m_LDBackend.createBinaryWriter();

  // initialize Relocator
  m_LDBackend.initRelocator(*m_pLinker);

  // initialize BranchIslandFactory
  m_LDBackend.initBRIslandFactory();

  // initialize StubFactory
  m_LDBackend.initStubFactory();

  // initialize target stubs
  m_LDBackend.initTargetStubs(*m_pLinker);
  return true;
}

/// initStdSections - initialize standard sections
bool ObjectLinker::initStdSections()
{
  ObjectBuilder builder(m_Config, m_Module);

  // initialize standard sections
  if (!m_LDBackend.initStdSections(builder))
    return false;

  // initialize target-dependent sections
  m_LDBackend.initTargetSections(m_Module, builder);

  return true;
}

void ObjectLinker::normalize()
{
  // -----  set up inputs  ----- //
  Module::input_iterator input, inEnd = m_Module.input_end();
  for (input = m_Module.input_begin(); input!=inEnd; ++input) {
    // is a group node
    if (isGroup(input)) {
      getGroupReader()->readGroup(input, m_Builder.getInputBuilder(), m_Config);
      continue;
    }

    // already got type - for example, bitcode or external OIR (object
    // intermediate representation)
    if ((*input)->type() == Input::Script ||
        (*input)->type() == Input::Archive ||
        (*input)->type() == Input::External)
      continue;

    if (Input::Object == (*input)->type()) {
      m_Module.getObjectList().push_back(*input);
      continue;
    }

    if (Input::DynObj == (*input)->type()) {
      m_Module.getLibraryList().push_back(*input);
      continue;
    }

    // read input as a binary file
    if (m_Config.options().isBinaryInput()) {
      (*input)->setType(Input::Object);
      getBinaryReader()->readBinary(**input);
      m_Module.getObjectList().push_back(*input);
    }
    // is a relocatable object file
    else if (getObjectReader()->isMyFormat(**input)) {
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
      Archive archive(**input, m_Builder.getInputBuilder());
      getArchiveReader()->readArchive(archive);
      if(archive.numOfObjectMember() > 0) {
        m_Module.getInputTree().merge<InputTree::Inclusive>(input,
                                                            archive.inputs());
      }
    }
    else {
      fatal(diag::err_unrecognized_input_file) << (*input)->path()
                                          << m_Config.targets().triple().str();
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

/// readRelocations - read all relocation entries
///
/// All symbols should be read and resolved before this function.
bool ObjectLinker::readRelocations()
{
  // Bitcode is read by the other path. This function reads relocation sections
  // in object files.
  mcld::InputTree::bfs_iterator input, inEnd = m_Module.getInputTree().bfs_end();
  for (input=m_Module.getInputTree().bfs_begin(); input!=inEnd; ++input) {
    if ((*input)->type() == Input::Object && (*input)->hasMemArea()) {
      if (!getObjectReader()->readRelocations(**input))
        return false;
    }
    // ignore the other kinds of files.
  }
  return true;
}

/// mergeSections - put allinput sections into output sections
bool ObjectLinker::mergeSections()
{
  ObjectBuilder builder(m_Config, m_Module);
  Module::obj_iterator obj, objEnd = m_Module.obj_end();
  for (obj = m_Module.obj_begin(); obj != objEnd; ++obj) {
    LDContext::sect_iterator sect, sectEnd = (*obj)->context()->sectEnd();
    for (sect = (*obj)->context()->sectBegin(); sect != sectEnd; ++sect) {
      switch ((*sect)->kind()) {
        // Some *INPUT sections should not be merged.
        case LDFileFormat::Ignore:
        case LDFileFormat::Null:
        case LDFileFormat::Relocation:
        case LDFileFormat::NamePool:
        case LDFileFormat::Group:
        case LDFileFormat::StackNote:
          // skip
          continue;
        case LDFileFormat::Target:
          if (!m_LDBackend.mergeSection(m_Module, **sect)) {
            error(diag::err_cannot_merge_section) << (*sect)->name()
                                                  << (*obj)->name();
            return false;
          }
          break;
        case LDFileFormat::EhFrame: {
          if (!(*sect)->hasEhFrame())
            continue; // skip

          if (!builder.MergeSection(**sect)) {
            error(diag::err_cannot_merge_section) << (*sect)->name()
                                                  << (*obj)->name();
            return false;
          }
          break;
        }
        default: {
          if (!(*sect)->hasSectionData())
            continue; // skip

          if (!builder.MergeSection(**sect)) {
            error(diag::err_cannot_merge_section) << (*sect)->name()
                                                  << (*obj)->name();
            return false;
          }
          break;
        }
      } // end of switch
    } // for each section
  } // for each obj
  return true;
}

/// addStandardSymbols - shared object and executable files need some
/// standard symbols
///   @return if there are some input symbols with the same name to the
///   standard symbols, return false
bool ObjectLinker::addStandardSymbols()
{
  // create and add section symbols for each output section
  Module::iterator iter, iterEnd = m_Module.end();
  for (iter = m_Module.begin(); iter != iterEnd; ++iter) {
    m_Module.getSectionSymbolSet().add(**iter, m_Module.getNamePool());
  }

  return m_LDBackend.initStandardSymbols(*m_pLinker, m_Module);
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

bool ObjectLinker::scanRelocations()
{
  // apply all relocations of all inputs
  Module::obj_iterator input, inEnd = m_Module.obj_end();
  for (input = m_Module.obj_begin(); input != inEnd; ++input) {
    LDContext::sect_iterator rs, rsEnd = (*input)->context()->relocSectEnd();
    for (rs = (*input)->context()->relocSectBegin(); rs != rsEnd; ++rs) {
      // bypass the reloc section if
      // 1. its section kind is changed to Ignore. (The target section is a
      // discarded group section.)
      // 2. it has no reloc data. (All symbols in the input relocs are in the
      // discarded group sections)
      if (LDFileFormat::Ignore == (*rs)->kind() || !(*rs)->hasRelocData())
        continue;
      RelocData::iterator reloc, rEnd = (*rs)->getRelocData()->end();
      for (reloc = (*rs)->getRelocData()->begin(); reloc != rEnd; ++reloc) {
        Relocation* relocation = llvm::cast<Relocation>(reloc);
        // scan relocation
        if (LinkerConfig::Object != m_Config.codeGenType()) {
          m_LDBackend.scanRelocation(*relocation,
                                     *m_pLinker,
                                     m_Module,
                                     *(*rs)->getLink());
        }
        else {
          m_LDBackend.partialScanRelocation(*relocation,
                                     *m_pLinker,
                                     m_Module,
                                     *(*rs)->getLink());
        }
      } // for all relocations
    } // for all relocation section
  } // for all inputs
  return true;
}

/// prelayout - help backend to do some modification before layout
bool ObjectLinker::prelayout()
{
  // finalize the section symbols, set their fragment reference and push them
  // into output symbol table
  Module::iterator sect, sEnd = m_Module.end();
  for (sect = m_Module.begin(); sect != sEnd; ++sect) {
    m_Module.getSectionSymbolSet().finalize(**sect, m_Module.getSymbolTable());
  }

  m_LDBackend.preLayout(m_Module, *m_pLinker);

  if (LinkerConfig::Object != m_Config.codeGenType() ||
      m_Config.options().isDefineCommon())
    m_LDBackend.allocateCommonSymbols(m_Module);

  /// check program interpreter - computer the name size of the runtime dyld
  if (!m_pLinker->isStaticLink() &&
      (LinkerConfig::Exec == m_Config.codeGenType() ||
       m_Config.options().isPIE() ||
       m_Config.options().hasDyld()))
    m_LDBackend.sizeInterp();

  /// measure NamePools - compute the size of name pool sections
  /// In ELF, will compute  the size of.symtab, .strtab, .dynsym, .dynstr,
  /// .hash and .shstrtab sections.
  ///
  /// dump all symbols and strings from FragmentLinker and build the format-dependent
  /// hash table.
  m_LDBackend.sizeNamePools(m_Module, m_pLinker->isStaticLink());

  return true;
}

/// layout - linearly layout all output sections and reserve some space
/// for GOT/PLT
///   Because we do not support instruction relaxing in this early version,
///   if there is a branch can not jump to its target, we return false
///   directly
bool ObjectLinker::layout()
{
  m_LDBackend.layout(m_Module, *m_pLinker);
  return true;
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
    case LinkerConfig::Binary:
      getBinaryWriter()->writeBinary(m_Module, pOutput);
      return true;
    default:
      fatal(diag::unrecognized_output_file) << m_Config.codeGenType();
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

