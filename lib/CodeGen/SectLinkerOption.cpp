//===- SectLinkerOption.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/CodeGen/SectLinkerOption.h"

#include <llvm/Module.h>
#include <llvm/Support/ErrorHandling.h>

#include "mcld/Support/DerivedPositionDependentOptions.h"
#include "mcld/Support/RealPath.h"

using namespace mcld;

//===----------------------------------------------------------------------===//
// SectLinkerOption
SectLinkerOption::SectLinkerOption(const llvm::Module &pModule,
                                   unsigned pInputModulePos,
                                   MCLDInfo &pLDInfo)
  : m_pLDInfo(&pLDInfo) {
  // read libraries the bitcode dependent from module
  for (llvm::Module::LibraryListType::const_iterator bitns = pModule.lib_begin(),
          bitnsEnd = pModule.lib_end(); bitns != bitnsEnd; ++bitns)
    appendOption(new NamespecOption(pInputModulePos, *bitns));
}

SectLinkerOption::~SectLinkerOption() {
  for (PositionDependentOptions::iterator pdoption = m_PosDepOptions.begin(),
       pdoptionEnd = m_PosDepOptions.end(); pdoption != pdoptionEnd; ++pdoption)
    delete *pdoption;
}
