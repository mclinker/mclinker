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
                                    const char* pName,
                                    Target::TripleMatchQualityFnTy pQualityFn)
{
  pTarget.Name = pName;
  pTarget.TripleMatchQualityFn = pQualityFn;

  s_TargetList.push_back(&pTarget);
}

const Target* TargetRegistry::lookupTarget(const std::string &pTriple,
                                           std::string &pError)
{
  if (empty()) {
    pError = "Unable to find target for this triple (no target are registered)";
    return NULL;
  }

  llvm::Triple triple(pTriple);
  Target* best = NULL, *ambiguity = NULL;
  unsigned int highest = 0;

  for (iterator target = begin(), ie = end(); target != ie; ++target) {
    unsigned int quality = (*target)->getTripleQuality(triple);
    if (quality > 0) {
      if (NULL == best || highest < quality) {
        highest = quality;
        best = *target;
        ambiguity = NULL;
      }
      else if (highest == quality) {
        ambiguity = *target;
      }
    }
  }

  if (NULL == best) {
    pError = "No availaible targets are compatible with this triple.";
    return NULL;
  }

  if (NULL != ambiguity) {
    pError = std::string("Ambiguous targets: \"") +
             best->name() + "\" and \"" + ambiguity->name() + "\"";
    return NULL;
  }

  return best;
}

