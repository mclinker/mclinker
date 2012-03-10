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

#include <mcld/ADT/BinTree.h>
#include <mcld/CodeGen/SectLinker.h>
#include <mcld/CodeGen/SectLinkerOption.h>
#include <mcld/MC/MCLDInputTree.h>
#include <mcld/MC/MCLDDriver.h>
#include <mcld/Support/DerivedPositionDependentOptions.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/Target/TargetLDBackend.h>

#include <llvm/Module.h>
#include <llvm/Support/ErrorHandling.h>
#include <llvm/Support/raw_ostream.h>

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
    m_pLDDriver(NULL) { }

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
}

bool SectLinker::doInitialization(Module &pM)
{
  MCLDInfo &info = m_pOption->info();

  // setup the output
  info.output().setContext(info.contextFactory().produce(info.output().path()));

  int mode = (Output::Object == info.output().type())? 0544 : 0755;
  info.output().setMemArea(
      info.memAreaFactory().produce(info.output().path(),
                                    O_RDWR | O_CREAT | O_TRUNC,
                                    mode));

  //   make sure output is openend successfully.
  if (!info.output().hasMemArea())
    report_fatal_error("output is not given on the command line\n");

  if (!info.output().memArea()->isGood())
    report_fatal_error("can not open output file :"+info.output().path().native());

  // let the target override the target-specific parameters
  addTargetOptions(pM, *m_pOption);

  // ----- convert position dependent options into tree of input files  ----- //
  PositionDependentOptions &PosDepOpts = m_pOption->pos_dep_options();
  std::stable_sort(PosDepOpts.begin(), PosDepOpts.end(), CompareOption);
  initializeInputTree(PosDepOpts);

  // Now, all input arguments are prepared well, send it into MCLDDriver
  m_pLDDriver = new MCLDDriver(info, *m_pLDBackend);

  return false;
}

bool SectLinker::doFinalization(Module &pM)
{
  const MCLDInfo &info = m_pOption->info();

  // 3. - initialize output's standard segments and sections
  if (!m_pLDDriver->initMCLinker())
    return true;

  // 4. - normalize the input tree
  m_pLDDriver->normalize();

  if (info.options().verbose()) {
    outs() << "MCLinker (LLVM Sub-project) - ";
    outs() << MCLDInfo::version();
    outs() << "\n";
  }

  if (info.options().trace()) {
    static int counter = 0;
    outs() << "** name\ttype\tpath\tsize (" << info.inputs().size() << ")\n";
    InputTree::const_dfs_iterator input, inEnd = info.inputs().dfs_end();
    for (input=info.inputs().dfs_begin(); input!=inEnd; ++input) {
      outs() << counter++ << " *  " << (*input)->name();
      switch((*input)->type()) {
      case Input::Archive:
        outs() << "\tarchive\t(";
        break;
      case Input::Object:
        outs() << "\tobject\t(";
        break;
      case Input::DynObj:
        outs() << "\tshared\t(";
        break;
      case Input::Script:
        outs() << "\tscript\t(";
        break;
      default:
        report_fatal_error("** Trace a unsupported file. It must be an internal bug!");
      }
      outs() << (*input)->path().c_str() << ")\n";
    }
  }

  // 5. - check if we can do static linking and if we use split-stack.
  if (!m_pLDDriver->linkable())
    return true;


  // 6. - read all sections
  if (!m_pLDDriver->readSections() ||
      !m_pLDDriver->mergeSections())
    return true;

  // 7. - read all symbol tables of input files and resolve them
  if (!m_pLDDriver->readSymbolTables() ||
      !m_pLDDriver->mergeSymbolTables())
    return true;

  // 7.a - add standard symbols and target-dependent symbols
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

  // 10.b - post-layout
  m_pLDDriver->postlayout();

  // 11. - finalize symbol value
  m_pLDDriver->finalizeSymbolValue();

  // 12. - apply relocations
  m_pLDDriver->relocate();

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

void SectLinker::initializeInputTree(const PositionDependentOptions &pPosDepOptions) const
{
  if (pPosDepOptions.empty())
    return;

  MCLDInfo &info = m_pOption->info();
  PositionDependentOptions::const_iterator cur_char = pPosDepOptions.begin();
  if (1 == pPosDepOptions.size() &&
      ((*cur_char)->type() != PositionDependentOption::INPUT_FILE &&
       (*cur_char)->type() != PositionDependentOption::NAMESPEC))
    return;

  InputTree::Connector *prev_ward = &InputTree::Downward;

  std::stack<InputTree::iterator> returnStack;
  InputTree::iterator cur_node = info.inputs().root();

  PositionDependentOptions::const_iterator charEnd = pPosDepOptions.end();
  while (cur_char != charEnd ) {
    switch ((*cur_char)->type()) {
    case PositionDependentOption::BITCODE: {
      // threat bitcode as a script in this version.
      const BitcodeOption *bitcode_option =
          static_cast<const BitcodeOption*>(*cur_char);
      info.inputs().insert(cur_node,
                           *prev_ward,
                           bitcode_option->path()->native(),
                           *(bitcode_option->path()),
                           Input::Script);
      info.setBitcode(**cur_node);
      prev_ward->move(cur_node);
      prev_ward = &InputTree::Afterward;
      break;
    }
    case PositionDependentOption::INPUT_FILE: {
      const InputFileOption *input_file_option =
          static_cast<const InputFileOption*>(*cur_char);
      info.inputs().insert(cur_node,
                           *prev_ward,
                           input_file_option->path()->native(),
                           *(input_file_option->path()));
      prev_ward->move(cur_node);
      prev_ward = &InputTree::Afterward;
      break;
    }
    case PositionDependentOption::NAMESPEC: {
      sys::fs::Path* path = 0;
      const NamespecOption *namespec_option =
          static_cast<const NamespecOption*>(*cur_char);
      if (info.attrFactory().last().isStatic()) {
        path = info.options().directories().find(namespec_option->namespec(),
                                                 Input::Archive);
      }
      else {
        path = info.options().directories().find(namespec_option->namespec(),
                                                 Input::DynObj);
      }

      if (0 == path) {
        llvm::report_fatal_error(std::string("Can't find namespec: ")+
                                 namespec_option->namespec());
      }
      info.inputs().insert(cur_node,
                           *prev_ward,
                           namespec_option->namespec(),
                           *path);
      prev_ward->move(cur_node);
      prev_ward = &InputTree::Afterward;
      break;
    }
    case PositionDependentOption::START_GROUP:
      info.inputs().enterGroup(cur_node, *prev_ward);
      prev_ward->move(cur_node);
      returnStack.push(cur_node);
      prev_ward = &InputTree::Downward;
      break;
    case PositionDependentOption::END_GROUP:
      cur_node = returnStack.top();
      returnStack.pop();
      prev_ward = &InputTree::Afterward;
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
      report_fatal_error("can not find the type of input file");
    }
    ++cur_char;
  }

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

