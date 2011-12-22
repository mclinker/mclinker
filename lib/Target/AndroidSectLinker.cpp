//===- AndroidSectLinker.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/Support/ErrorHandling.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Module.h>
#include <mcld/Target/AndroidSectLinker.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/MC/MCLDDriver.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLDDirectory.h>

using namespace mcld;

//==========================
// AndroidSectLinker

AndroidSectLinker::AndroidSectLinker(const llvm::cl::opt<std::string> &pInputFilename,
                                     const std::string &pOutputFilename,
                                     unsigned int OutputLinkType,
                                     MCLDInfo &pLDInfo,
                                     TargetLDBackend &pLDBackend)
  : SectLinker(pInputFilename,
               pOutputFilename,
               OutputLinkType,
               pLDInfo,
               pLDBackend) {
}

AndroidSectLinker::~AndroidSectLinker()
{
  // SectLinker will delete m_pLDBackend and m_pLDDriver;
}

void AndroidSectLinker::addInputsBeforeCMD(llvm::Module &pM,
                                           MCLDInfo& pLDInfo,
                                           SectLinker::PositionDependentOptions &pOptions)
{
  // -----  Set up General Options  ----- //
  MCLDDirectory search_path("=/system/lib");
  search_path.setSysroot(pLDInfo.options().sysroot());
  if (exists(search_path.path()) && is_directory(search_path.path()))
    pLDInfo.options().directories().add(search_path);
  else {
    // FIXME: need a warning function
    llvm::errs() << "WARNING: can not open search directory: `-L" << search_path.name() << "'.\n";
  }
}

