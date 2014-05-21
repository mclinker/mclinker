//===- MipsELFMCLinker.h --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TARGET_MIPS_MIPSELFMCLINKER_H
#define TARGET_MIPS_MIPSELFMCLINKER_H
#include <mcld/Target/ELFMCLinker.h>

namespace mcld {

class Module;
class FileHandle;

/** \class MipsELFMCLinker
 *  \brief MipsELFMCLinker sets up the environment for linking.
 */
class MipsELFMCLinker : public ELFMCLinker
{
public:
  MipsELFMCLinker(LinkerConfig& pConfig,
                  mcld::Module& pModule,
                  FileHandle& pFileHandle);

  ~MipsELFMCLinker();
};

} // namespace of mcld

#endif

