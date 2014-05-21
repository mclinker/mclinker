//===- ELFMCLinker.h ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// ELFMCLinker is a customized linker pass for ELF platform.
// This pass set up default parameters for ELF.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TARGET_ELFMCLINKER_H
#define MCLD_TARGET_ELFMCLINKER_H
#include <mcld/CodeGen/MCLinker.h>

namespace mcld {

class Module;
class Output;
class FileHandle;

class ELFMCLinker : public MCLinker
{
public:
  ELFMCLinker(LinkerConfig& pConfig,
              mcld::Module& pModule,
              FileHandle& pFileHandle);

  virtual ~ELFMCLinker();
};

} // namespace of mcld

#endif

