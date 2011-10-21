//===- SectLinker.cpp -----------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/ADT/BinTree.h"
#include "mcld/CodeGen/SectLinker.h"
#include "mcld/MC/MCLDDriver.h"
#include "mcld/MC/MCLDInfo.h"
#include "mcld/MC/MCLDInputTree.h"
#include "mcld/MC/MCLDDirectory.h"
#include "mcld/Support/CommandLine.h"
#include "mcld/Support/FileSystem.h"
#include "mcld/Support/RealPath.h"
#include "mcld/Target/TargetLDBackend.h"

#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <stack>
#include <string>

#ifdef MCLD_DEBUG
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/FileSystem.h"
#endif

using namespace mcld;
using namespace mcld::sys::fs;
using namespace llvm;

//===----------------------------------------------------------------------===//
// Forward declarations
char SectLinker::m_ID = 0;
bool compare_options(const SectLinker::PositionDependentOption* X,
                     const SectLinker::PositionDependentOption* Y);

//===----------------------------------------------------------------------===//
// Command Line Options
// There are four kinds of command line options:
//   1. input, (may be a file, such as -m and /tmp/XXXX.o.)
//   2. attribute of inputs, (describing the attributes of inputs, such as
//      --as-needed and --whole-archive. usually be positional.)
//   3. scripting options, (represent a subset of link scripting language, such
//      as --defsym.)
//   4. and general options. (the rest of options)
//===----------------------------------------------------------------------===//
// General Options
static cl::opt<mcld::sys::fs::Path, false, llvm::cl::parser<mcld::sys::fs::Path> >
ArgSysRoot("sysroot",
           cl::desc("Use directory as the location of the sysroot, overriding the configure-time default."),
           cl::value_desc("directory"),
           cl::ValueRequired);

static cl::list<mcld::MCLDDirectory, bool, llvm::cl::parser<mcld::MCLDDirectory> >
ArgSearchDirList("L",
                 cl::ZeroOrMore,
                 cl::desc("Add path searchdir to the list of paths that ld will search for archive libraries and ld control scripts."),
                 cl::value_desc("searchdir"),
                 cl::Prefix);

static cl::alias
ArgSearchDirListAlias("library-path",
                      cl::desc("alias for -L"),
                      cl::aliasopt(ArgSearchDirList));

static cl::opt<bool>
ArgTrace("t",
         cl::desc("Print the names of the input files as ld processes them."));

static cl::alias
ArgTraceAlias("trace",
              cl::desc("alias for -t"),
              cl::aliasopt(ArgTrace));

static cl::opt<bool>
ArgVerbose("V",
          cl::desc("Display the version number for ld and list the linker emulations supported."));

static cl::alias
ArgVerboseAlias("verbose",
                cl::desc("alias for -V"),
                cl::aliasopt(ArgVerbose));

//===----------------------------------------------------------------------===//
// Inputs
static cl::list<mcld::sys::fs::Path>
ArgInputObjectFiles(cl::Positional,
                    cl::desc("[input object files]"),
                    cl::ZeroOrMore);

static cl::list<std::string>
ArgNameSpecList("l",
            cl::ZeroOrMore,
            cl::desc("Add the archive or object file specified by namespec to the list of files to link."),
            cl::value_desc("namespec"),
            cl::Prefix);

static cl::alias
ArgNameSpecListAlias("library",
                 cl::desc("alias for -l"),
                 cl::aliasopt(ArgNameSpecList));

static cl::list<bool>
ArgStartGroupList("start-group",
                  cl::ValueDisallowed,
                  cl::desc("start to record a group of archives"));

static cl::alias
ArgStartGroupListAlias("(",
                       cl::desc("alias for --start-group"),
                       cl::aliasopt(ArgStartGroupList));

static cl::list<bool>
ArgEndGroupList("end-group",
                cl::ValueDisallowed,
                cl::desc("stop recording a group of archives"));

static cl::alias
ArgEndGroupListAlias(")",
                     cl::desc("alias for --end-group"),
                     cl::aliasopt(ArgEndGroupList));

//===----------------------------------------------------------------------===//
// Attributes of Inputs
static cl::list<bool>
ArgWholeArchiveList("whole-archive",
                    cl::ValueDisallowed,
                    cl::desc("For each archive mentioned on the command line after the --whole-archive option, include all object files in the archive."));

static cl::list<bool>
ArgNoWholeArchiveList("no-whole-archive",
                    cl::ValueDisallowed,
                    cl::desc("Turn off the effect of the --whole-archive option for subsequent archive files."));

static cl::list<bool>
ArgAsNeededList("as-needed",
                cl::ValueDisallowed,
                cl::desc("This option affects ELF DT_NEEDED tags for dynamic libraries mentioned on the command line after the --as-needed option."));

static cl::list<bool>
ArgNoAsNeededList("no-as-needed",
                cl::ValueDisallowed,
                cl::desc("Turn off the effect of the --as-needed option for subsequent dynamic libraries"));

static cl::list<bool>
ArgAddNeededList("add-needed",
                cl::ValueDisallowed,
                cl::desc("--add-needed causes DT_NEEDED tags are always emitted for those libraries from DT_NEEDED tags. This is the default behavior."));

static cl::list<bool>
ArgNoAddNeededList("no-add-needed",
                cl::ValueDisallowed,
                cl::desc("--no-add-needed causes DT_NEEDED tags will never be emitted for those libraries from DT_NEEDED tags"));

static cl::list<bool>
ArgBDynamicList("Bdynamic",
                cl::ValueDisallowed,
                cl::desc("Link against dynamic library"));

static cl::alias
ArgBDynamicListAlias1("dy",
                     cl::desc("alias for --Bdynamic"),
                     cl::aliasopt(ArgBDynamicList));

static cl::alias
ArgBDynamicListAlias2("call_shared",
                     cl::desc("alias for --Bdynamic"),
                     cl::aliasopt(ArgBDynamicList));

static cl::list<bool>
ArgBStaticList("Bstatic",
                cl::ValueDisallowed,
                cl::desc("Link against static library"));

static cl::alias
ArgBStaticListAlias1("dn",
                     cl::desc("alias for --Bstatic"),
                     cl::aliasopt(ArgBStaticList));

static cl::alias
ArgBStaticListAlias2("static",
                     cl::desc("alias for --Bstatic"),
                     cl::aliasopt(ArgBStaticList));

static cl::alias
ArgBStaticListAlias3("non_shared",
                     cl::desc("alias for --Bstatic"),
                     cl::aliasopt(ArgBStaticList));

//===----------------------------------------------------------------------===//
// Scripting Options

//===----------------------------------------------------------------------===//
// SectLinker
SectLinker::SectLinker(const std::string& pInputFile,
                       const std::string& pOutputFile,
                       unsigned int pOutputLinkType,
                       MCLDInfo& pLDInfo,
                       TargetLDBackend& pLDBackend)
  : MachineFunctionPass(m_ID),
    m_LDInfo(pLDInfo),
    m_pLDBackend(&pLDBackend) {
  // create the default input, and assign the default attribute to it.
  m_LDInfo.inputs().insert<InputTree::Inclusive>(m_LDInfo.inputs().root(),
                                                 "default bitcode",
                                                 RealPath(pInputFile),
                                                 Input::Object);
  Input* bitcode = *m_LDInfo.inputs().begin();
  bitcode->setContext(m_LDInfo.contextFactory().produce(bitcode->path()));
  m_LDInfo.setBitcode(*bitcode);

  // create the default output
  m_LDInfo.output().setType(pOutputLinkType);
  m_LDInfo.output().setPath(sys::fs::RealPath(pOutputFile));
  m_LDInfo.output().setContext(
                          m_LDInfo.contextFactory().produce(
                                                   m_LDInfo.output().path()));

  int mode = (Output::Object == m_LDInfo.output().type())? 0666 : 0777;
  m_LDInfo.output().setMemArea(
                          m_LDInfo.memAreaFactory().produce(
                                                   m_LDInfo.output().path(),
                                                   O_RDWR | O_CREAT | O_TRUNC,
                                                   mode));

  // general options
}

SectLinker::~SectLinker()
{
  delete m_pLDBackend;
  delete m_pLDDriver;
}

bool SectLinker::doInitialization(Module &pM)
{
  // -----  Set up General Options  ----- //
  //   make sure output is openend successfully.
  if (!m_LDInfo.output().hasMemArea())
    report_fatal_error("output is not given on the command line\n");

  if (!m_LDInfo.output().memArea()->isGood())
    report_fatal_error("can not open output file :"+m_LDInfo.output().path().native());

  //   set up sysroot
  if (!ArgSysRoot.empty()) {
    if (exists(ArgSysRoot) && is_directory(ArgSysRoot))
      m_LDInfo.options().setSysroot(ArgSysRoot);
  }

  // add all search directories
  cl::list<mcld::MCLDDirectory>::iterator sd;
  cl::list<mcld::MCLDDirectory>::iterator sdEnd = ArgSearchDirList.end();
  for (sd=ArgSearchDirList.begin(); sd!=sdEnd; ++sd) {
    if (sd->isInSysroot())
      sd->setSysroot(m_LDInfo.options().sysroot());
    if (exists(sd->path()) && is_directory(sd->path())) {
      m_LDInfo.options().directories().add(*sd);
    }
    else {
      // FIXME: need a warning function
      errs() << "search directory is wrong: -L" << sd->name();
    }
  }

  // set up trace
  m_LDInfo.options().setTrace(ArgTrace);

  // set up verbose
  m_LDInfo.options().setVerbose(ArgVerbose);

  // -----  Set up Inputs  ----- //
  unsigned int input_size = ArgNameSpecList.size() +
                            ArgStartGroupList.size() +
                            ArgEndGroupList.size() +
                            ArgInputObjectFiles.size();

  PositionDependentOptions pos_dep_options;
  pos_dep_options.reserve(input_size);

  // add all start-group
  cl::list<bool>::iterator sg;
  cl::list<bool>::iterator sgEnd = ArgStartGroupList.end();
  for (sg=ArgStartGroupList.begin(); sg!=sgEnd; ++sg) {
    // calculate position
    pos_dep_options.push_back(new PositionDependentOption(
                                    ArgStartGroupList.getPosition(sg-ArgStartGroupList.begin()),
                                    PositionDependentOption::START_GROUP));
  }

  // add all end-group
  cl::list<bool>::iterator eg;
  cl::list<bool>::iterator egEnd = ArgEndGroupList.end();
  for (eg=ArgEndGroupList.begin(); eg!=egEnd; ++eg) {
    // calculate position
    pos_dep_options.push_back(new PositionDependentOption(
                                    ArgEndGroupList.getPosition(eg-ArgEndGroupList.begin()),
                                    PositionDependentOption::END_GROUP));
  }

  // add all namespecs
  cl::list<std::string>::iterator ns;
  cl::list<std::string>::iterator nsEnd = ArgNameSpecList.end();
  for (ns=ArgNameSpecList.begin(); ns!=nsEnd; ++ns) {
    // calculate position
    pos_dep_options.push_back(new PositionDependentOption(
                                    ArgNameSpecList.getPosition(ns-ArgNameSpecList.begin()),
                                    *ns,
                                    PositionDependentOption::NAMESPEC));
  }

  // add all object files
  cl::list<mcld::sys::fs::Path>::iterator obj;
  cl::list<mcld::sys::fs::Path>::iterator objEnd = ArgInputObjectFiles.end();
  for (obj=ArgInputObjectFiles.begin(); obj!=objEnd; ++obj) {
    // calculate position
    pos_dep_options.push_back(new PositionDependentOption(
                                    ArgInputObjectFiles.getPosition(obj-ArgInputObjectFiles.begin()),
                                    *obj,
                                    PositionDependentOption::INPUT_FILE));
  }

  // -----  Set up Attributes of Inputs  ----- //
  // --whole-archive
  cl::list<bool>::iterator attr = ArgWholeArchiveList.begin();
  cl::list<bool>::iterator attrEnd = ArgWholeArchiveList.end();
  for (; attr!=attrEnd; ++attr) {
    pos_dep_options.push_back(new PositionDependentOption(
                                    ArgWholeArchiveList.getPosition(attr-ArgWholeArchiveList.begin()),
                                    PositionDependentOption::WHOLE_ARCHIVE));
  }

  // --no-whole-archive
  attr = ArgNoWholeArchiveList.begin();
  attrEnd = ArgNoWholeArchiveList.end();
  for (; attr!=attrEnd; ++attr) {
    pos_dep_options.push_back(new PositionDependentOption(
                                    ArgNoWholeArchiveList.getPosition(attr-ArgNoWholeArchiveList.begin()),
                                    PositionDependentOption::NO_WHOLE_ARCHIVE));
  }

  // --as-needed
  attr = ArgAsNeededList.begin();
  attrEnd = ArgAsNeededList.end();
  while(attr != attrEnd) {
    pos_dep_options.push_back(new PositionDependentOption(
                                    ArgAsNeededList.getPosition(attr-ArgAsNeededList.begin()),
                                    PositionDependentOption::AS_NEEDED));
    ++attr;
  }

  // --no-as-needed
  attr = ArgNoAsNeededList.begin();
  attrEnd = ArgNoAsNeededList.end();
  while(attr != attrEnd) {
    pos_dep_options.push_back(new PositionDependentOption(
                                    ArgNoAsNeededList.getPosition(attr-ArgNoAsNeededList.begin()),
                                    PositionDependentOption::NO_AS_NEEDED));
    ++attr;
  }

  // --add-needed
  attr = ArgAddNeededList.begin();
  attrEnd = ArgAddNeededList.end();
  while(attr != attrEnd) {
    pos_dep_options.push_back(new PositionDependentOption(
                                    ArgAddNeededList.getPosition(attr-ArgAddNeededList.begin()),
                                    PositionDependentOption::ADD_NEEDED));
    ++attr;
  }

  // --no-add-needed
  attr = ArgNoAddNeededList.begin();
  attrEnd = ArgNoAddNeededList.end();
  while(attr != attrEnd) {
    pos_dep_options.push_back(new PositionDependentOption(
                                    ArgNoAddNeededList.getPosition(attr-ArgNoAddNeededList.begin()),
                                    PositionDependentOption::NO_ADD_NEEDED));
    ++attr;
  }

  // -Bdynamic
  attr = ArgBDynamicList.begin();
  attrEnd = ArgBDynamicList.end();
  while(attr != attrEnd) {
    pos_dep_options.push_back(new PositionDependentOption(
                                    ArgBDynamicList.getPosition(attr-ArgBDynamicList.begin()),
                                    PositionDependentOption::BDYNAMIC));
    ++attr;
  }

  // -Bstatic
  attr = ArgBStaticList.begin();
  attrEnd = ArgBStaticList.end();
  while(attr != attrEnd) {
    pos_dep_options.push_back(new PositionDependentOption(
                                    ArgBStaticList.getPosition(attr-ArgBStaticList.begin()),
                                    PositionDependentOption::BSTATIC));
    ++attr;
  }

  // -----  Set up Scripting Options  ----- //

  // ----- convert position dependent options into tree of input files  ----- //
  std::stable_sort(pos_dep_options.begin(), pos_dep_options.end(), compare_options);
  initializeInputTree(m_LDInfo, pos_dep_options);

  // Now, all input arguments are prepared well, send it into MCLDDriver
  m_pLDDriver = new MCLDDriver(m_LDInfo, *m_pLDBackend);

}

bool SectLinker::doFinalization(Module &pM)
{
  // 3. - normalize the input tree
  m_pLDDriver->normalize();

  if (m_LDInfo.options().verbose()) {
    outs() << "MCLinker (LLVM Sub-project) - ";
    outs() << MCLDInfo::version();
    outs() << "\n";
  }

  if (m_LDInfo.options().trace()) {
    static int counter = 0;
    outs() << "** name\ttype\tpath\tsize (" << m_LDInfo.inputs().size() << ")\n";
    InputTree::dfs_iterator input, inEnd = m_LDInfo.inputs().dfs_end();
    for (input=m_LDInfo.inputs().dfs_begin(); input!=inEnd; ++input) {
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

  // 4. and 5. - check if we can do static linking and if we use split-stack.
  if (!m_pLDDriver->linkable())
    return true;

  // 6. - read all symbol tables of input files.
  m_pLDDriver->readSymbolTables();

  // 7. - symbol resolution
  m_pLDDriver->mergeSymbolTables();

  // 8. - add undefined symbols
//  m_pLDDriver->addUndefinedSymbols();

  
/**
  m_pLDDriver->relocation();
  m_pLDDriver->writeOut();
**/
}

bool SectLinker::runOnMachineFunction(MachineFunction& pF)
{
  // basically, linkers do nothing during function is generated.
}

void SectLinker::initializeInputTree(MCLDInfo& pLDInfo,
                        const PositionDependentOptions &pPosDepOptions) const
{
  if (pPosDepOptions.empty())
    return;

  PositionDependentOptions::const_iterator cur_char = pPosDepOptions.begin();
  if (1 == pPosDepOptions.size() &&
      ((*cur_char)->type() != PositionDependentOption::INPUT_FILE ||
      (*cur_char)->type() != PositionDependentOption::NAMESPEC))
    return;

  InputTree::Connector *prev_ward = &InputTree::Afterward;

  std::stack<InputTree::iterator> returnStack;
  InputTree::iterator cur_node = pLDInfo.inputs().begin();

  PositionDependentOptions::const_iterator charEnd = pPosDepOptions.end();
  while (cur_char != charEnd ) {
    switch ((*cur_char)->type()) {
    case PositionDependentOption::INPUT_FILE:
      pLDInfo.inputs().insert(cur_node,
                     *prev_ward,
                     (*cur_char)->path()->native(),
                     *(*cur_char)->path());
      prev_ward->move(cur_node);
      prev_ward = &InputTree::Afterward;
      break;
    case PositionDependentOption::NAMESPEC: {
      Path* path = 0;
      if (pLDInfo.attrFactory().last().isStatic()) {
        path = pLDInfo.options().directories().find(
                                               (*cur_char)->namespec(),
                                               Input::Archive);
      }
      else {
        path = pLDInfo.options().directories().find(
                                               (*cur_char)->namespec(),
                                               Input::DynObj);
      }

      if (0 == path) {
        llvm::report_fatal_error(std::string("Can't find namespec: ")+
                                 (*cur_char)->namespec());
      }
      pLDInfo.inputs().insert(cur_node,
                     *prev_ward,
                     (*cur_char)->namespec(),
                     *path);
      prev_ward->move(cur_node);
      prev_ward = &InputTree::Afterward;
      break;
    }
    case PositionDependentOption::START_GROUP:
      pLDInfo.inputs().enterGroup(cur_node, *prev_ward);
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
      pLDInfo.attrFactory().last().setWholeArchive();
      break;
    case PositionDependentOption::NO_WHOLE_ARCHIVE:
      pLDInfo.attrFactory().last().unsetWholeArchive();
      break;
    case PositionDependentOption::AS_NEEDED:
      pLDInfo.attrFactory().last().setAsNeeded();
      break;
    case PositionDependentOption::NO_AS_NEEDED:
      pLDInfo.attrFactory().last().unsetAsNeeded();
      break;
    case PositionDependentOption::ADD_NEEDED:
      pLDInfo.attrFactory().last().setAddNeeded();
      break;
    case PositionDependentOption::NO_ADD_NEEDED:
      pLDInfo.attrFactory().last().unsetAddNeeded();
      break;
    case PositionDependentOption::BSTATIC:
      pLDInfo.attrFactory().last().setStatic();
      break;
    case PositionDependentOption::BDYNAMIC:
      pLDInfo.attrFactory().last().setDynamic();
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
// PositionDependentOption
SectLinker::PositionDependentOption::PositionDependentOption(
                              unsigned int pPosition,
                              SectLinker::PositionDependentOption::Type pType)
  : m_Type(pType), m_Position(pPosition), m_pPath(0), m_pNamespec() {
}

SectLinker::PositionDependentOption::PositionDependentOption(
                              unsigned int pPosition,
                              const sys::fs::Path& pInputFile,
                              SectLinker::PositionDependentOption::Type pType)
  : m_Type(pType),
    m_Position(pPosition),
    m_pPath(&pInputFile),
    m_pNamespec() {
}

SectLinker::PositionDependentOption::PositionDependentOption(
                              unsigned int pPosition,
                              const std::string& pNamespec,
                              SectLinker::PositionDependentOption::Type pType)
  : m_Type(pType),
    m_Position(pPosition),
    m_pPath(0),
    m_pNamespec(pNamespec.data()) {
}

//===----------------------------------------------------------------------===//
// Non-member functions
bool compare_options(const SectLinker::PositionDependentOption* X,
                     const SectLinker::PositionDependentOption* Y)
{
  return (X->position() < Y->position());
}

