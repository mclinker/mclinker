//===- AArch64ELFMCLinker.h -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TARGET_AARCH64_AARCH64ELFMCLINKER_H
#define TARGET_AARCH64_AARCH64ELFMCLINKER_H
#include <mcld/Target/ELFMCLinker.h>

namespace mcld {

class Module;
class FileHandle;

/** \class AArch64ELFMCLinker
 *  \brief AArch64ELFMCLinker sets up the environment for linking.
 */
class AArch64ELFMCLinker : public ELFMCLinker
{
public:
  AArch64ELFMCLinker(LinkerConfig& pConfig,
                     mcld::Module& pModule,
                     FileHandle& pFileHandle);

  ~AArch64ELFMCLinker();
};

} // namespace of mcld

#endif

