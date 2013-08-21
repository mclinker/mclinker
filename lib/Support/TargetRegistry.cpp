//===- TargetRegistry.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/TargetRegistry.h>

using namespace mcld;

TargetRegistry::TargetListTy mcld::TargetRegistry::s_TargetList;

//===----------------------------------------------------------------------===//
// TargetRegistry
//===----------------------------------------------------------------------===//
void TargetRegistry::RegisterTarget(Target& pTarget,
                                    Target::TripleMatchQualityFnTy pQualityFn)
{
  pTarget.TripleMatchQualityFn = pQualityFn;

  s_TargetList.push_back(&pTarget);
}

const mcld::Target*
mcld::TargetRegistry::lookupTarget(const std::string &pTriple,
                                   std::string &pError)
{
  const llvm::Target* target = llvm::TargetRegistry::lookupTarget(pTriple, pError);
  if (!target)
    return NULL;

  mcld::Target *result = 0;
  TargetListTy::const_iterator TIter, TEnd = s_TargetList.end();
  for (TIter=s_TargetList.begin(); TIter!=TEnd; ++TIter) {
    if ((*TIter)->get() == target) {
      result = (*TIter);
      break;
    }
  }
  return result;
}

