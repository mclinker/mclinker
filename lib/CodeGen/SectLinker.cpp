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
// Arguments
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

static cl::list<mcld::sys::fs::Path>
ArgInputObjectFiles(cl::Positional,
                    cl::desc("[input object files]"),
                    cl::ZeroOrMore);

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
    ldInfo->setDefaultBitcode(*m_pDefaultBitcode);

  /// set up all target-independent parameters into MCLDInfo
  //   set up sysroot
  if (!ArgSysRoot.empty()) {
    if (exists(ArgSysRoot) && is_directory(ArgSysRoot))
      ldInfo->setSysroot(ArgSysRoot);
  }
  
  /// add all search directories
  OwningPtr<SearchDirs> search_dir_list;
  search_dir_list.reset(new SearchDirs());
  cl::list<mcld::MCLDDirectory>::iterator sd;
  cl::list<mcld::MCLDDirectory>::iterator sdEnd = ArgSearchDirList.end();
  for (sd=ArgSearchDirList.begin(); sd!=sdEnd; ++sd) {
    if (sd->isInSysroot()) 
      sd->setSysroot(ArgSysRoot);
    if (exists(sd->path()) && is_directory(sd->path()))
      search_dir_list.get()->add(*sd);
    else
      errs() << "search directory is wrong: -L" << sd->name();
  }

  /// add all namespecs

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

