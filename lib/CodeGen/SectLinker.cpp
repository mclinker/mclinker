//===- SectLinker.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the SectLinker class.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/FileHandle.h>
#include <mcld/MC/InputTree.h>
#include <mcld/MC/MCLDDriver.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Support/FileHandle.h>
#include <mcld/Support/raw_ostream.h>
#include <mcld/Support/MemoryAreaFactory.h>
#include <mcld/Support/DerivedPositionDependentOptions.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/CodeGen/SectLinker.h>
#include <mcld/CodeGen/SectLinkerOption.h>

#include <llvm/Module.h>

#include <algorithm>
#include <stack>
#include <string>

using namespace mcld;
using namespace llvm;

//===----------------------------------------------------------------------===//
// Forward declarations
char SectLinker::m_ID = 0;
static bool CompareOption(const PositionDependentOption* X,
                          const PositionDependentOption* Y);

//===----------------------------------------------------------------------===//
// SectLinker
SectLinker::SectLinker(SectLinkerOption &pOption,
                       TargetLDBackend& pLDBackend)
  : MachineFunctionPass(m_ID),
    m_pOption(&pOption),
    m_pLDBackend(&pLDBackend),
    m_pLDDriver(NULL),
    m_pMemAreaFactory(NULL)
{
  m_pMemAreaFactory = new MemoryAreaFactory(32);
}

SectLinker::~SectLinker()
{
  delete m_pLDDriver;

  // FIXME: current implementation can not change the order of delete.
  //
  // Instance of TargetLDBackend was created outside and is not managed by
  // SectLinker. It should not be destroyed here and by SectLinker. However, in
  // order to follow the LLVM convention - that is, the pass manages all the
  // objects it used during the processing, we destroy the object of
  // TargetLDBackend here.
  delete m_pLDBackend;

  delete m_pMemAreaFactory;
}

bool SectLinker::doInitialization(Module &pM)
{
  MCLDInfo &info = m_pOption->info();

  // ----- convert position dependent options into tree of input files  ----- //
  PositionDependentOptions &PosDepOpts = m_pOption->pos_dep_options();
  std::stable_sort(PosDepOpts.begin(), PosDepOpts.end(), CompareOption);
  initializeInputTree(PosDepOpts);
  initializeInputOutput(info);
  // Now, all input arguments are prepared well, send it into MCLDDriver
  m_pLDDriver = new MCLDDriver(info, *m_pLDBackend, *memAreaFactory());

  return false;
}

bool SectLinker::doFinalization(Module &pM)
{
  const MCLDInfo &info = m_pOption->info();

  // 2. - initialize MCLinker
  if (!m_pLDDriver->initMCLinker())
    return true;

  // 3. - initialize output's standard sections
  if (!m_pLDDriver->initStdSections())
    return true;

  // 4. - normalize the input tree
  m_pLDDriver->normalize();

  if (info.options().trace()) {
    static int counter = 0;
    mcld::outs() << "** name\ttype\tpath\tsize (" << info.inputs().size() << ")\n";
    InputTree::const_dfs_iterator input, inEnd = info.inputs().dfs_end();
    for (input=info.inputs().dfs_begin(); input!=inEnd; ++input) {
      mcld::outs() << counter++ << " *  " << (*input)->name();
      switch((*input)->type()) {
      case Input::Archive:
        mcld::outs() << "\tarchive\t(";
        break;
      case Input::Object:
        mcld::outs() << "\tobject\t(";
        break;
      case Input::DynObj:
        mcld::outs() << "\tshared\t(";
        break;
      case Input::Script:
        mcld::outs() << "\tscript\t(";
        break;
      case Input::External:
        mcld::outs() << "\textern\t(";
        break;
      default:
        unreachable(diag::err_cannot_trace_file) << (*input)->type()
                                                 << (*input)->name()
                                                 << (*input)->path();
      }
      mcld::outs() << (*input)->path() << ")\n";
    }
  }

  // 5. - check if we can do static linking and if we use split-stack.
  if (!m_pLDDriver->linkable())
    return true;


  // 6. - merge all sections
  if (!m_pLDDriver->mergeSections())
    return true;

  // 7. - add standard symbols and target-dependent symbols
  // m_pLDDriver->addUndefSymbols();
  if (!m_pLDDriver->addStandardSymbols() ||
      !m_pLDDriver->addTargetSymbols())
    return true;

  // 8. - read all relocation entries from input files
  m_pLDDriver->readRelocations();

  // 9. - pre-layout
  m_pLDDriver->prelayout();

  // 10. - linear layout
  m_pLDDriver->layout();

  // 10.b - post-layout (create segment, instruction relaxing)
  m_pLDDriver->postlayout();

  // 11. - finalize symbol value
  m_pLDDriver->finalizeSymbolValue();

  // 12. - apply relocations
  m_pLDDriver->relocation();

  // 13. - write out output
  m_pLDDriver->emitOutput();

  // 14. - post processing
  m_pLDDriver->postProcessing();
  return false;
}

bool SectLinker::runOnMachineFunction(MachineFunction& pF)
{
  // basically, linkers do nothing during function is generated.
  return false;
}

void SectLinker::initializeInputOutput(MCLDInfo &pLDInfo)
{
  // -----  initialize output file  ----- //
  FileHandle::Permission perm;
  if (Output::Object == pLDInfo.output().type())
    perm = 0544;
  else
    perm = 0755;

  MemoryArea* out_area = memAreaFactory()->produce(pLDInfo.output().path(),
                                                 FileHandle::ReadWrite,
                                                 perm);

  if (!out_area->handler()->isGood()) {
    // make sure output is openend successfully.
    fatal(diag::err_cannot_open_output_file) << pLDInfo.output().name()
                                             << pLDInfo.output().path();
  }

  pLDInfo.output().setMemArea(out_area);
  pLDInfo.output().setContext(pLDInfo.contextFactory().produce());

  // -----  initialize input files  ----- //
  InputTree::dfs_iterator input, inEnd = pLDInfo.inputs().dfs_end();
  for (input = pLDInfo.inputs().dfs_begin(); input!=inEnd; ++input) {
    // already got type - for example, bitcode
    if ((*input)->type() == Input::Script ||
        (*input)->type() == Input::Object ||
        (*input)->type() == Input::DynObj  ||
        (*input)->type() == Input::Archive)
      continue;

    MemoryArea *input_memory =
        memAreaFactory()->produce((*input)->path(), FileHandle::ReadOnly);

    if (input_memory->handler()->isGood()) {
      (*input)->setMemArea(input_memory);
    }
    else {
      error(diag::err_cannot_open_input) << (*input)->name() << (*input)->path();
      return;
    }

    LDContext *input_context =
        pLDInfo.contextFactory().produce((*input)->path());

    (*input)->setContext(input_context);
  }
}

void SectLinker::initializeInputTree(const PositionDependentOptions &pPosDepOptions) const
{
  if (pPosDepOptions.empty())
    fatal(diag::err_no_inputs);

  MCLDInfo &info = m_pOption->info();
  PositionDependentOptions::const_iterator option = pPosDepOptions.begin();
  if (1 == pPosDepOptions.size() &&
      ((*option)->type() != PositionDependentOption::INPUT_FILE &&
       (*option)->type() != PositionDependentOption::NAMESPEC) &&
       (*option)->type() != PositionDependentOption::BITCODE) {
    // if we only have one positional options, and the option is
    // not an input file, then emit error message.
    fatal(diag::err_no_inputs);
  }

  // -----  Input tree insertion algorithm  ----- //
  //   The type of the previsou node indicates the direction of the current
  //   insertion.
  //
  //     root   : the parent node who being inserted.
  //     mover  : the direcion of current movement.
  //
  //   for each positional options:
  //     insert the options in current root.
  //     calculate the next movement

  // Initialization
  InputTree::Mover *move = &InputTree::Downward;
  InputTree::iterator root = info.inputs().root();
  PositionDependentOptions::const_iterator optionEnd = pPosDepOptions.end();
  std::stack<InputTree::iterator> returnStack;

  while (option != optionEnd ) {

    switch ((*option)->type()) {
      /** bitcode **/
      case PositionDependentOption::BITCODE: {

        const BitcodeOption *bitcode_option =
            static_cast<const BitcodeOption*>(*option);

        // threat bitcode as an external IR in this version.
        info.inputs().insert(root, *move,
                             bitcode_option->path()->native(),
                             *(bitcode_option->path()),
                             Input::External);

        info.setBitcode(**root);

        // move root on the new created node.
        move->move(root);

        // the next file is appended after bitcode file.
        move = &InputTree::Afterward;
        break;
      }

      /** input object file **/
      case PositionDependentOption::INPUT_FILE: {
        const InputFileOption *input_file_option =
            static_cast<const InputFileOption*>(*option);

        info.inputs().insert(root, *move,
                             input_file_option->path()->native(),
                             *(input_file_option->path()));

        // move root on the new created node.
        move->move(root);

        // the next file is appended after object file.
        move = &InputTree::Afterward;
        break;
      }

    /** -lnamespec **/
    case PositionDependentOption::NAMESPEC: {
      sys::fs::Path* path = NULL;
      const NamespecOption *namespec_option =
          static_cast<const NamespecOption*>(*option);

      // find out the real path of the namespec.
      if (info.attrFactory().constraint().isSharedSystem()) {
        // In the system with shared object support, we can find both archive
        // and shared object.

        if (info.attrFactory().last().isStatic()) {
          // with --static, we must search an archive.
          path = info.options().directories().find(namespec_option->namespec(),
                                                   Input::Archive);
        }
        else {
          // otherwise, with --Bdynamic, we can find either an archive or a
          // shared object.
          path = info.options().directories().find(namespec_option->namespec(),
                                                   Input::DynObj);
        }
      }
      else {
        // In the system without shared object support, we only look for an
        // archive.
        path = info.options().directories().find(namespec_option->namespec(),
                                                 Input::Archive);
      }

      if (NULL == path)
        fatal(diag::err_cannot_find_namespec) << namespec_option->namespec();

      info.inputs().insert(root, *move,
                           namespec_option->namespec(),
                           *path);

      // iterate root on the new created node.
      move->move(root);

      // the file after a namespec must be appended afterward.
      move = &InputTree::Afterward;
      break;
    }

    /** start group **/
    case PositionDependentOption::START_GROUP:
      info.inputs().enterGroup(root, *move);
      move->move(root);
      returnStack.push(root);
      move = &InputTree::Downward;
      break;
    /** end group **/
    case PositionDependentOption::END_GROUP:
      root = returnStack.top();
      returnStack.pop();
      move = &InputTree::Afterward;
      break;
    case PositionDependentOption::WHOLE_ARCHIVE:
      info.attrFactory().last().setWholeArchive();
      break;
    case PositionDependentOption::NO_WHOLE_ARCHIVE:
      info.attrFactory().last().unsetWholeArchive();
      break;
    case PositionDependentOption::AS_NEEDED:
      info.attrFactory().last().setAsNeeded();
      break;
    case PositionDependentOption::NO_AS_NEEDED:
      info.attrFactory().last().unsetAsNeeded();
      break;
    case PositionDependentOption::ADD_NEEDED:
      info.attrFactory().last().setAddNeeded();
      break;
    case PositionDependentOption::NO_ADD_NEEDED:
      info.attrFactory().last().unsetAddNeeded();
      break;
    case PositionDependentOption::BSTATIC:
      info.attrFactory().last().setStatic();
      break;
    case PositionDependentOption::BDYNAMIC:
      info.attrFactory().last().setDynamic();
      break;
    default:
      fatal(diag::err_cannot_identify_option) << (*option)->position()
                                              << (uint32_t)(*option)->type();
    } // end of switch
    ++option;
  } // end of while

  if (!returnStack.empty()) {
    report_fatal_error("no matched --start-group and --end-group");
  }
}

//===----------------------------------------------------------------------===//
// Non-member functions
static bool CompareOption(const PositionDependentOption* X,
                          const PositionDependentOption* Y)
{
  return (X->position() < Y->position());
}

