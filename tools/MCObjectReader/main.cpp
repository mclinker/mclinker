/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <Jush.Lu@mediatek.com>                                            *
 ****************************************************************************/

#include "llvm/ADT/OwningPtr.h"
#include "llvm/Bitcode/BitstreamReader.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/system_error.h"

#include "mcld/MC/MCELFObjectReader.h"
#include "mcld/MC/MCELFObjectTargetReader.h"
#include "mcld/MC/MCLDFile.h"

using namespace llvm;
using namespace mcld;
using namespace std;

static cl::opt<std::string>
InputFilename(cl::Positional, cl::desc("<input bitcode>"), cl::init("-"));


int main(int argc, char** argv) {
  cl::ParseCommandLineOptions(argc, argv, "MCObjectReader");

  MCLDFile File;
  MCLDContext *LDContext = new MCLDContext();
  File.setContext(LDContext);

  MCELFObjectTargetReader *TargetReader;
  MCELFObjectReader Reader(TargetReader);

  Reader.readObject(InputFilename, File);

  File.context()->dump();

  return 0;
}
