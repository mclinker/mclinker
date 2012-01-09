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

#include "mcld/ADT/BinTree.h"
#include "mcld/CodeGen/SectLinker.h"
#include "mcld/CodeGen/SectLinkerOption.h"
#include "mcld/MC/MCLinker.h"
#include "mcld/MC/MCLDInfo.h"
#include "mcld/MC/MCLDInputTree.h"
#include "mcld/MC/MCLDDirectory.h"
#include "mcld/MC/MCLDDriver.h"
#include "mcld/Support/CommandLine.h"
#include "mcld/Support/FileSystem.h"
#include "mcld/Support/RealPath.h"
#include "mcld/Support/DerivedPositionDependentOptions.h"
#include "mcld/Target/TargetLDBackend.h"

#include <llvm/CodeGen/AsmPrinter.h>
#include <llvm/CodeGen/MachineFunction.h>
#include <llvm/MC/MCObjectWriter.h>
#include <llvm/Module.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/ErrorHandling.h>
#include <llvm/Support/raw_ostream.h>

#include <algorithm>
#include <stack>
#include <string>

#ifdef MCLD_DEBUG
#include <llvm/ADT/SmallVector.h>
#include <llvm/Support/FileSystem.h>
#endif

using namespace mcld;
using namespace mcld::sys::fs;
using namespace llvm;

//===----------------------------------------------------------------------===//
// Forward declarations
char SectLinker::m_ID = 0;
static bool CompareOption(const PositionDependentOption* X,
                          const PositionDependentOption* Y);

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

static cl::opt<std::string>
ArgEntry("e",
         cl::desc("Use entry as the explicit symbol for beginning execution of your program."),
         cl::value_desc("entry"),
         cl::ValueRequired);

static cl::alias
ArgEntryAlias("entry",
              cl::desc("alias for -e"),
              cl::aliasopt(ArgEntry));

static cl::opt<bool>
Bsymbolic("Bsymbolic",
          cl::desc("Bind references within the shared library."),
          cl::init(false));

static cl::opt<std::string>
SONAME("soname",
       cl::desc("Set internal name of shared library"),
       cl::value_desc("name"));

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
SectLinker::SectLinker(SectLinkerOption &pOption,
                       const llvm::cl::opt<std::string>& pInputFile,
                       const std::string& pOutputFile,
                       unsigned int pOutputLinkType,
                       TargetLDBackend& pLDBackend)
  : MachineFunctionPass(m_ID),
    m_pOption(&pOption),
    m_pLDBackend(&pLDBackend),
    m_InputBitcode(pInputFile) {

  MCLDInfo &info = m_pOption->info();

  // create the default output
  info.output().setSONAME(SONAME);
  info.output().setType(pOutputLinkType);
  info.output().setPath(sys::fs::RealPath(pOutputFile));
  info.output().setContext(info.contextFactory().produce(info.output().path()));

  int mode = (Output::Object == info.output().type())? 0544 : 0755;
  info.output().setMemArea(
                          info.memAreaFactory().produce(
                                                   info.output().path(),
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
  MCLDInfo &info = m_pOption->info();

  // -----  Set up General Options  ----- //
  //   make sure output is openend successfully.
  if (!info.output().hasMemArea())
    report_fatal_error("output is not given on the command line\n");

  if (!info.output().memArea()->isGood())
    report_fatal_error("can not open output file :"+info.output().path().native());

  //   set up sysroot
  if (!ArgSysRoot.empty()) {
    if (exists(ArgSysRoot) && is_directory(ArgSysRoot))
      info.options().setSysroot(ArgSysRoot);
  }

  // add all search directories
  cl::list<mcld::MCLDDirectory>::iterator sd;
  cl::list<mcld::MCLDDirectory>::iterator sdEnd = ArgSearchDirList.end();
  for (sd=ArgSearchDirList.begin(); sd!=sdEnd; ++sd) {
    if (sd->isInSysroot())
      sd->setSysroot(info.options().sysroot());
    if (exists(sd->path()) && is_directory(sd->path())) {
      info.options().directories().add(*sd);
    }
    else {
      // FIXME: need a warning function
      errs() << "WARNING: can not open search directory `-L"
             << sd->name()
             << "'.\n";
    }
  }

  info.options().setTrace(ArgTrace);

  info.options().setVerbose(ArgVerbose);

  info.options().setEntry(ArgEntry);

  info.options().setBsymbolic(Bsymbolic);

  // -----  Set up Inputs  ----- //
  unsigned int input_size = ArgNameSpecList.size() +
                            ArgStartGroupList.size() +
                            ArgEndGroupList.size() +
                            ArgInputObjectFiles.size();

  PositionDependentOptions &pos_dep_options = m_pOption->pos_dep_options();
  pos_dep_options.reserve(input_size);

  // override the parameters before all positional options
  addInputsBeforeCMD(pM, *m_pOption);

  // -----  read bitcode  -----//
  // add bitcode input, and assign the default attribute to it.
  RealPath *bitcode_path = new RealPath(m_InputBitcode);
  pos_dep_options.push_back(new BitcodeOption(m_InputBitcode.getPosition(),
                                              *bitcode_path));

  // add all start-group
  cl::list<bool>::iterator sg;
  cl::list<bool>::iterator sgEnd = ArgStartGroupList.end();
  for (sg=ArgStartGroupList.begin(); sg!=sgEnd; ++sg) {
    // calculate position
    pos_dep_options.push_back(new StartGroupOption(
                                    ArgStartGroupList.getPosition(sg-ArgStartGroupList.begin())));
  }

  // add all end-group
  cl::list<bool>::iterator eg;
  cl::list<bool>::iterator egEnd = ArgEndGroupList.end();
  for (eg=ArgEndGroupList.begin(); eg!=egEnd; ++eg) {
    // calculate position
    pos_dep_options.push_back(new EndGroupOption(
                                    ArgEndGroupList.getPosition(eg-ArgEndGroupList.begin())));
  }

  // add all namespecs
  cl::list<std::string>::iterator ns;
  cl::list<std::string>::iterator nsEnd = ArgNameSpecList.end();
  for (ns=ArgNameSpecList.begin(); ns!=nsEnd; ++ns) {
    // calculate position
    pos_dep_options.push_back(new NamespecOption(
                                    ArgNameSpecList.getPosition(ns-ArgNameSpecList.begin()),
                                    *ns));
  }

  // add all object files
  cl::list<mcld::sys::fs::Path>::iterator obj;
  cl::list<mcld::sys::fs::Path>::iterator objEnd = ArgInputObjectFiles.end();
  for (obj=ArgInputObjectFiles.begin(); obj!=objEnd; ++obj) {
    // calculate position
    pos_dep_options.push_back(new InputFileOption(
                                    ArgInputObjectFiles.getPosition(obj-ArgInputObjectFiles.begin()),
                                    *obj));
  }

  // -----  Set up Attributes of Inputs  ----- //
  // --whole-archive
  cl::list<bool>::iterator attr = ArgWholeArchiveList.begin();
  cl::list<bool>::iterator attrEnd = ArgWholeArchiveList.end();
  for (; attr!=attrEnd; ++attr) {
    pos_dep_options.push_back(new WholeArchiveOption(
                                    ArgWholeArchiveList.getPosition(attr-ArgWholeArchiveList.begin())));
  }

  // --no-whole-archive
  attr = ArgNoWholeArchiveList.begin();
  attrEnd = ArgNoWholeArchiveList.end();
  for (; attr!=attrEnd; ++attr) {
    pos_dep_options.push_back(new NoWholeArchiveOption(
                                    ArgNoWholeArchiveList.getPosition(attr-ArgNoWholeArchiveList.begin())));
  }

  // --as-needed
  attr = ArgAsNeededList.begin();
  attrEnd = ArgAsNeededList.end();
  while(attr != attrEnd) {
    pos_dep_options.push_back(new AsNeededOption(
                                    ArgAsNeededList.getPosition(attr-ArgAsNeededList.begin())));
    ++attr;
  }

  // --no-as-needed
  attr = ArgNoAsNeededList.begin();
  attrEnd = ArgNoAsNeededList.end();
  while(attr != attrEnd) {
    pos_dep_options.push_back(new NoAsNeededOption(
                                    ArgNoAsNeededList.getPosition(attr-ArgNoAsNeededList.begin())));
    ++attr;
  }

  // --add-needed
  attr = ArgAddNeededList.begin();
  attrEnd = ArgAddNeededList.end();
  while(attr != attrEnd) {
    pos_dep_options.push_back(new AddNeededOption(
                                    ArgAddNeededList.getPosition(attr-ArgAddNeededList.begin())));
    ++attr;
  }

  // --no-add-needed
  attr = ArgNoAddNeededList.begin();
  attrEnd = ArgNoAddNeededList.end();
  while(attr != attrEnd) {
    pos_dep_options.push_back(new NoAddNeededOption(
                                    ArgNoAddNeededList.getPosition(attr-ArgNoAddNeededList.begin())));
    ++attr;
  }

  // -Bdynamic
  attr = ArgBDynamicList.begin();
  attrEnd = ArgBDynamicList.end();
  while(attr != attrEnd) {
    pos_dep_options.push_back(new BDynamicOption(
                                    ArgBDynamicList.getPosition(attr-ArgBDynamicList.begin())));
  }

  // -Bstatic
  attr = ArgBStaticList.begin();
  attrEnd = ArgBStaticList.end();
  while(attr != attrEnd) {
    pos_dep_options.push_back(new BStaticOption(
                                    ArgBStaticList.getPosition(attr-ArgBStaticList.begin())));
    ++attr;
  }

  // -----  Set up Scripting Options  ----- //

  // override the parameters after all positional options
  addInputsAfterCMD(pM, *m_pOption);

  // ----- convert position dependent options into tree of input files  ----- //
  PositionDependentOptions &PosDepOpts = m_pOption->pos_dep_options();
  std::stable_sort(PosDepOpts.begin(), PosDepOpts.end(), CompareOption);
  initializeInputTree(PosDepOpts);

  // Now, all input arguments are prepared well, send it into MCLDDriver
  m_pLDDriver = new MCLDDriver(info, *m_pLDBackend);

  // clear up positional dependent options
  PositionDependentOptions::iterator pdoption, pdoptionEnd = pos_dep_options.end();
  for (pdoption = pos_dep_options.begin(); pdoption != pdoptionEnd; ++pdoption)
    delete *pdoption;
  delete bitcode_path;
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

  // 8. - read all relocation entries from input files
  m_pLDDriver->readRelocations();

  // 7.a - add standard symbols and target-dependent symbols
  // m_pLDDriver->addUndefSymbols();
  if (!m_pLDDriver->addStandardSymbols() ||
      !m_pLDDriver->addTargetSymbols())
    return true;

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
      ((*cur_char)->type() != PositionDependentOption::INPUT_FILE ||
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
      Path* path = 0;
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

