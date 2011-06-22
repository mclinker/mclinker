#include "ARMTargetMachine.h"
#include "ARMMCAsmInfo.h"
#include "ARMFrameLowering.h"
#include "ARM.h"
#include "llvm/PassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Target/TargetOptions.h"

//mcld
#include "ARMLDBackend.h"
#include "mcld/Target/TargetRegistry.h"

using namespace llvm;

static MCLDDriver *createARMLDDriver() {

} 

static MCLDBackend *createARMLDBackend() {

}


extern "C" void LLVMInitializeARMTarget() {

  RegisterTargetMachine<ARMTargetMachine> X(TheARMTarget);

  TargetRegistry::RegisterLDBackend(TheARMTarget,createARMLDBackend);  

  // Register the LD Driver.
  TargetRegistry::RegisterLDDrive(TheARMTarget,createARMLDDriver);

}
