//===- mcld.cpp -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <stdlib.h>

#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/CommandLine.h>

#include <alone/Config/Config.h>
#include <alone/Support/Initialization.h>
#include <alone/Linker.h>

using namespace alone;

// Override "mcld -version"
void MCLDVersionPrinter() {
  llvm::raw_ostream &os = llvm::outs();
  os << "mcld (The MCLinker Project, http://mclinker.googlecode.com/):\n"
     << "  Default target: " << DEFAULT_TARGET_TRIPLE_STRING << "\n";

  os << "\n";

  os << "LLVM (http://llvm.org/):\n";
  return;
}

static inline
bool ConfigLinker(Linker &pLinker)
{
  return true;
}

int main(int argc, char* argv[])
{
  llvm::cl::SetVersionPrinter(MCLDVersionPrinter);
  llvm::cl::ParseCommandLineOptions(argc, argv);
  init::Initialize();

  Linker linker;
  if (!ConfigLinker(linker)) {
    return EXIT_FAILURE;
  }
}

