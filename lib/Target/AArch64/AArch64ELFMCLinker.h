//===- AArch64ELFMCLinker.h -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef AARCH64_ELF_SECTION_LINKER_H
#define AARCH64_ELF_SECTION_LINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Target/ELFMCLinker.h>

namespace mcld {

class Module;
class MemoryArea;

/** \class AArch64ELFMCLinker
 *  \brief AArch64ELFMCLinker sets up the environment for linking.
 */
class AArch64ELFMCLinker : public ELFMCLinker
{
public:
  AArch64ELFMCLinker(LinkerConfig& pConfig,
                     mcld::Module& pModule,
                     MemoryArea& pOutput);

  ~AArch64ELFMCLinker();
};

} // namespace of mcld

#endif

