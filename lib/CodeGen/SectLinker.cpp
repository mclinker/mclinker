/*****************************************************************************
 *   The MC Linker Project, Copyright (C), 2011 -                            *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Csmon Lu <chun-hung.lu@mediatek.com>                                    *
 *   Luba Tang <luba.tang@mediatek.com>                                      *
 ****************************************************************************/
#include <llvm/Module.h>
#include <llvm/CodeGen/AsmPrinter.h>
#include <llvm/MC/MCObjectWriter.h>
#include <llvm/CodeGen/MachineFunction.h>
#include <llvm/Support/CommandLine.h>
#include <mcld/CodeGen/SectLinker.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/MC/MCLDDriver.h>
#include <mcld/MC/MCLDFile.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/MC/MCLDDirectory.h>
#include <mcld/Support/CommandLine.h>
#include <algorithm>
#include <stack>

#ifdef MCLD_DEBUG
#include <llvm/ADT/SmallVector.h>
#include <llvm/Support/FileSystem.h>
#include <iostream>
using namespace std;
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

static cl::list<std::string>
ArgStartGroupList("start-group",
                  cl::ZeroOrMore,
                  cl::ValueDisallowed,
                  cl::desc("start to record a group of archives"),
                  cl::value_desc("archives"),
                  cl::Prefix);
static cl::alias
ArgStartGroupListAlias("(",
                       cl::desc("alias for --start-group"),
                       cl::aliasopt(ArgStartGroupList));

static cl::list<std::string>
ArgEndGroupList("end-group",
                cl::ZeroOrMore,
                cl::ValueDisallowed,
                cl::desc("stop recording a group of archives"),
                cl::value_desc("archives"),
                cl::Prefix);

static cl::alias
ArgEndGroupListAlias(")",
                     cl::desc("alias for --end-group"),
                     cl::aliasopt(ArgEndGroupList));

//===----------------------------------------------------------------------===//
// Attributes of Inputs

//===----------------------------------------------------------------------===//
// Scripting Options

//===----------------------------------------------------------------------===//
// SectLinker
SectLinker::SectLinker(MCLDInfo& pLDInfo,
                       TargetLDBackend& pLDBackend)
  : MachineFunctionPass(m_ID),
    m_LDInfo(pLDInfo),
    m_pLDBackend(&pLDBackend) {
}

SectLinker::~SectLinker()
{
  delete m_pLDBackend;
  delete m_pLDDriver;
}

bool SectLinker::doInitialization(Module &pM)
{
  /// -----  Set up General Options  ----- //
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
    if (exists(sd->path()) && is_directory(sd->path()))
      m_LDInfo.options().directories().add(*sd);
    else {
      // FIXME: need a warning function
      errs() << "search directory is wrong: -L" << sd->name();
    }
  }

  // set up trace
  m_LDInfo.options().setTrace(ArgTrace);

  /// -----  Set up Inputs  -----
  unsigned int input_size = ArgNameSpecList.size() +
                            ArgStartGroupList.size() +
                            ArgEndGroupList.size() +
                            ArgInputObjectFiles.size();
                            
  PositionDependentOptions pos_dep_options;
  pos_dep_options.reserve(input_size);

  // add all namespecs
  cl::list<std::string>::iterator ns;
  cl::list<std::string>::iterator nsEnd = ArgNameSpecList.end();
  for (ns=ArgNameSpecList.begin(); ns!=nsEnd; ++ns) {
    // search file in SearchDirs
    SearchDirs::iterator library = m_LDInfo.options().directories().find(*ns);
    if (m_LDInfo.options().directories().end()!=library) { // found
      // calculate position
      pos_dep_options.push_back(new PositionDependentOption(
                                      ArgNameSpecList.getPosition(ns-ArgNameSpecList.begin()),
                                      *library,
                                      *ns,
                                      PositionDependentOption::NAMESPEC));
    }
    else {
      // FIXME: need a fetal error function
      errs() << "can not find " << (*ns);
      exit (1);
    }
  }

  /// add all start-group
  cl::list<std::string>::iterator sg;
  cl::list<std::string>::iterator sgEnd = ArgStartGroupList.end();
  for (sg=ArgStartGroupList.begin(); sg!=sgEnd; ++sg) {
    // calculate position
    pos_dep_options.push_back(new PositionDependentOption(
                                    ArgStartGroupList.getPosition(sg-ArgStartGroupList.begin()),
                                    PositionDependentOption::START_GROUP));
  }

  /// add all end-group
  cl::list<std::string>::iterator eg;
  cl::list<std::string>::iterator egEnd = ArgEndGroupList.end();
  for (eg=ArgEndGroupList.begin(); eg!=egEnd; ++eg) {
    // calculate position
    pos_dep_options.push_back(new PositionDependentOption(
                                    ArgEndGroupList.getPosition(eg-ArgEndGroupList.begin()),
                                    PositionDependentOption::END_GROUP));
  }

  /// add all object files
  cl::list<mcld::sys::fs::Path>::iterator obj;
  cl::list<mcld::sys::fs::Path>::iterator objEnd = ArgInputObjectFiles.end();
  for (obj=ArgInputObjectFiles.begin(); obj!=objEnd; ++obj) {
    // calculate position
    pos_dep_options.push_back(new PositionDependentOption(
                                    ArgInputObjectFiles.getPosition(obj-ArgInputObjectFiles.begin()),
                                    *obj,
                                    PositionDependentOption::INPUT_FILE));
  }

  /// -----  Set up Attributes of Inputs  ----- //
  /// -----  Set up Scripting Options  ----- //

  /// ----- convert position dependent options into tree of input files  ----- //
  std::stable_sort(pos_dep_options.begin(), pos_dep_options.end(), compare_options);
  initializeInputTree(m_LDInfo.inputs(), pos_dep_options);
  m_pLDDriver = new MCLDDriver(*m_pLDBackend);
}

bool SectLinker::doFinalization(Module &pM)
{
  // get MCSectionData
  // real linking
  // emit object file
}

bool SectLinker::runOnMachineFunction(MachineFunction& pF)
{
}

void SectLinker::initializeInputTree(InputTree& pInputs,
                        const PositionDependentOptions &pPosDepOptions) const
{
  if (pPosDepOptions.empty())
    return;

  PositionDependentOptions::const_iterator cur_char = pPosDepOptions.begin();
  if (1 == pPosDepOptions.size() && 
      ((*cur_char)->type() == PositionDependentOption::START_GROUP ||
       (*cur_char)->type() == PositionDependentOption::END_GROUP))
    return;

  InputTree::Connector *prev_ward = &InputTree::Downward;

  std::stack<InputTree::iterator> returnStack;
  switch ((*cur_char)->type()) {
  case PositionDependentOption::INPUT_FILE:
    pInputs.insert(pInputs.root(),
                   InputTree::Input,
                   "file",
                   *(*cur_char)->path(),
                   *prev_ward);
    prev_ward = &InputTree::Afterward;
    break;
  case PositionDependentOption::NAMESPEC:
    pInputs.insert(pInputs.root(),
                   InputTree::Input,
                   (*cur_char)->namespec(),
                   *(*cur_char)->path(),
                   *prev_ward);
    prev_ward = &InputTree::Afterward;
    break;
  case PositionDependentOption::START_GROUP:
    pInputs.enterGroup(pInputs.root(),
                       *prev_ward);
    returnStack.push(pInputs.begin());
    prev_ward = &InputTree::Downward;
    break;
  case PositionDependentOption::END_GROUP:
  default:
    // FIXME: need a fetal error function
    errs() << "illegal syntax of input files";
    exit (1);
  }

  // --- //
  InputTree::iterator cur_node = pInputs.begin();
  PositionDependentOptions::const_iterator charEnd = pPosDepOptions.end();
  ++cur_char;
  
  while (cur_char != charEnd ) {
    switch ((*cur_char)->type()) {
    case PositionDependentOption::INPUT_FILE:
      pInputs.insert(cur_node,
                     InputTree::Input,
                     "file",
                     *(*cur_char)->path(),
                     *prev_ward);
      prev_ward = &InputTree::Afterward;
      break;
    case PositionDependentOption::NAMESPEC:
      pInputs.insert(cur_node,
                     InputTree::Input,
                     (*cur_char)->namespec(),
                     *(*cur_char)->path(),
                     *prev_ward);
      prev_ward = &InputTree::Afterward;
      break;
    case PositionDependentOption::START_GROUP:
      pInputs.enterGroup(cur_node, *prev_ward);
      returnStack.push(cur_node);
      prev_ward = &InputTree::Downward;
      break;
    case PositionDependentOption::END_GROUP:
      cur_node = returnStack.top();
      returnStack.pop();
      prev_ward = &InputTree::Afterward;
      break;
    default:
      // FIXME: need a fetal error function
      errs() << "can not find the type of input file";
      exit (1);
    }
    ++cur_char;
  }

  if (!returnStack.empty()) {
    // FIXME: need a fetal error function
    errs() << "no matched --start-group and --end-group";
    exit (1);
  }
}

//===----------------------------------------------------------------------===//
// Non-member functions
bool compare_options(const SectLinker::PositionDependentOption* X,
                     const SectLinker::PositionDependentOption* Y)
{
  return (X->position() < Y->position());
}
