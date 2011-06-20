/*****************************************************************************
 *   The BOLD Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@gmail.com>                                            *
 ****************************************************************************/
#include <mcld/Target/TargetMachine.h>
#include <llvm/PassManager.h>
#include <llvm/CodeGen/AsmPrinter.h>
#include <llvm/CodeGen/Passes.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/MC/MCAsmInfo.h>
#include <llvm/MC/MCStreamer.h>

using namespace mcld;
using namespace llvm;

namespace mcld
{

//==========================
// TargetMachine
bool addPassesToEmitFile(TargetMachine &pTarget,
			   PassManagerBase &pPM,
			   formatted_raw_ostream &pOS,
			   mcld::CodeGenFileType pFileType,
			   CodeGenOpt::Level pOptLvl,
			   bool pDisableVerify ) 
{
  MCContext* Context = 0;
  if (pTarget.addCommonCodeGenPasses( pPM, pOptLvl, pDisableVerify, Context))
    return true;

  switch( pFileType ) {
  default: return true;
  case CGFT_DSOFile: {
    // create MCLDPrinter
    // PM.add();
    break;
  }
  case CGFT_EXEFile: {
    break;
  }
  case mcld::CGFT_Null: {
    assert("Null File");
    break;
  }
  }
}

} // namespace of mcld
