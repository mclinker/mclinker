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

#ifdef MCLD_DEBUG
#include <llvm/ADT/SmallVector.h>
#include <llvm/Support/FileSystem.h>
#include <iostream>
using namespace std;
#endif

using namespace mcld;
using namespace mcld::sys::fs;
using namespace llvm;

char SectLinker::m_ID = 0;

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
SectLinker::SectLinker(TargetLDBackend& pLDBackend, MCLDFile* pDefaultBitcode)
  : MachineFunctionPass(m_ID),
    m_pLDBackend(&pLDBackend),
    m_pDefaultBitcode(pDefaultBitcode) {
}

SectLinker::~SectLinker()
{
  delete m_pLDBackend;
  delete m_pLDDriver;
  delete m_pDefaultBitcode;
}

bool SectLinker::doInitialization(Module &pM)
{
  // create MCLDInfo
  MCLDInfo *ldInfo = createLDInfo();
  if (0 != m_pDefaultBitcode)
    ldInfo->options()->setDefaultBitcode(*m_pDefaultBitcode);

  /// -----  Set up General Options  -----
  //   set up sysroot
  if (!ArgSysRoot.empty()) {
    if (exists(ArgSysRoot) && is_directory(ArgSysRoot))
      ldInfo->options()->setSysroot(ArgSysRoot);
  }
  
  // add all search directories
  cl::list<mcld::MCLDDirectory>::iterator sd;
  cl::list<mcld::MCLDDirectory>::iterator sdEnd = ArgSearchDirList.end();
  for (sd=ArgSearchDirList.begin(); sd!=sdEnd; ++sd) {
    if (sd->isInSysroot()) 
      sd->setSysroot(ldInfo->options()->sysroot());
    if (exists(sd->path()) && is_directory(sd->path()))
      ldInfo->options()->directories()->add(*sd);
    else {
      // FIXME: need a warning function
      errs() << "search directory is wrong: -L" << sd->name();
    }
  }

  /// -----  Set up Inputs  -----
  unsigned int input_size = ArgNameSpecList.size() +
                            ArgStartGroupList.size() +
                            ArgEndGroupList.size() +
                            ArgInputObjectFiles.size();
                            
  PositionDependentOptions pos_dep_options(input_size);

  // add all namespecs
  cl::list<std::string>::iterator ns;
  cl::list<std::string>::iterator nsEnd = ArgNameSpecList.end();
  for (ns=ArgNameSpecList.begin(); ns!=nsEnd; ++ns) {
    // search file in SearchDirs
    SearchDirs::iterator library = ldInfo->options()->directories()->find(*ns);
    if (ldInfo->options()->directories()->end()!=library) { // found
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

  /// -----  Set up Attributes of Inputs  -----
  /// -----  Set up Scripting Options  -----

  /// ----- convert position dependent options into tree of input files  -----
  m_pLDDriver = new MCLDDriver(*ldInfo, *m_pLDBackend);
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

