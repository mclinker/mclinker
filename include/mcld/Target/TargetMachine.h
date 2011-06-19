/*****************************************************************************
 *   The BOLD Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@gmail.com>                                            *
 ****************************************************************************/
#ifndef TARGETMACHINE_H
#define TARGETMACHINE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/Target/TargetMachine.h>

namespace llvm
{
class PassManagerBase;
class formatted_raw_ostream;

} // namespace of llvm

namespace mcld
{

using namespace llvm;

enum CodeGenFileType {
  CGFT_Null = 2,
  CGFT_DSOFile,
  CGFT_EXEFile
};

extern "C" bool addPassesToEmitFile(TargetMachine &,
			   PassManagerBase &,
			   formatted_raw_ostream &,
			   mcld::CodeGenFileType,
			   CodeGenOpt::Level,
			   bool = true); 

} // namespace of BOLD

#endif

