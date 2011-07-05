/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@gmail.com>                                          *
 ****************************************************************************/
#include <mcld/Target/TargetRegistry.h>

using namespace std;

mcld::TargetRegistry::TargetListTy mcld::TargetRegistry::s_TargetList;

/* ** */

void mcld::TargetRegistry::RegisterTarget(mcld::Target &T)
{
  s_TargetList.push_back(&T);
}

const mcld::Target* mcld::TargetRegistry::lookupTarget(const llvm::Target &pTarget)
{
  mcld::Target *result = 0;
  TargetListTy::const_iterator TIter, TEnd = s_TargetList.end();
  for (TIter=s_TargetList.begin(); TIter!=TEnd; ++TIter) {
    if ((*TIter)->get()==&pTarget) {
      result = (*TIter);
      break;
    }
  }
  return result;
}

const mcld::Target *mcld::TargetRegistry::lookupTarget(const std::string &pTriple,
                                                       std::string &pError) 
{
  const llvm::Target* target = llvm::TargetRegistry::lookupTarget( pTriple, pError );
  if (!target)
    return 0;
  return lookupTarget( *target );
}

