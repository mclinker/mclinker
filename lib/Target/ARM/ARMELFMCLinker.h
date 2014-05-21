//===- ARMELFMCLinker.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TARGET_ARM_ARMELFMCLINKER_H
#define TARGET_ARM_ARMELFMCLINKER_H
#include <mcld/Target/ELFMCLinker.h>

namespace mcld {

class Module;
class FileHandle;

/** \class ARMELFMCLinker
 *  \brief ARMELFMCLinker sets up the environment for linking.
 */
class ARMELFMCLinker : public ELFMCLinker
{
public:
  ARMELFMCLinker(LinkerConfig& pConfig,
                 mcld::Module& pModule,
                 FileHandle& pFileHandle);

  ~ARMELFMCLinker();
};

} // namespace of mcld

#endif

