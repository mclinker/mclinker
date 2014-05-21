//===- X86ELFMCLinker.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TARGET_X86_X86ELFMCLINKER_H
#define TARGET_X86_X86ELFMCLINKER_H
#include <mcld/Target/ELFMCLinker.h>

namespace mcld {

class Module;
class FileHandle;

/** \class X86ELFMCLinker
 *  \brief X86ELFMCLinker sets up the environment for linking.
 *
 *  \see
 */
class X86ELFMCLinker : public ELFMCLinker
{
public:
  X86ELFMCLinker(LinkerConfig& pConfig,
                 mcld::Module& pModule,
                 FileHandle& pFileHandle);

  ~X86ELFMCLinker();
};

} // namespace of mcld

#endif

