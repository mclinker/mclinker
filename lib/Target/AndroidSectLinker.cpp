//===- AndroidSectLinker.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/Target/AndroidSectLinker.h>

#include <llvm/Support/ErrorHandling.h>
#include <mcld/MC/MCLDDirectory.h>
#include <mcld/CodeGen/SectLinkerOption.h>

using namespace mcld;

//==========================
// AndroidSectLinker

AndroidSectLinker::AndroidSectLinker(SectLinkerOption &pOption,
                                     TargetLDBackend &pLDBackend)
  : SectLinker(pOption, pLDBackend) {
}

AndroidSectLinker::~AndroidSectLinker()
{
  // SectLinker will delete m_pLDBackend and m_pLDDriver;
}

void AndroidSectLinker::addTargetOptions(llvm::Module &pM,
                                         SectLinkerOption &pOption)
{
  // -----  Set up General Options  ----- //
  MCLDInfo &info = pOption.info();
  MCLDDirectory search_path("=/system/lib");
  search_path.setSysroot(info.options().sysroot());
  if (exists(search_path.path()) && is_directory(search_path.path()))
    info.options().directories().add(search_path);
  else {
    // FIXME: need a warning function
    llvm::errs() << "WARNING: can not open search directory: `-L" << search_path.name() << "'.\n";
  }
}

