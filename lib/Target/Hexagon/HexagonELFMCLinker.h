//===- HexagonELFMCLinker.h -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TARGET_HEXAGON_HEXAGONELFMCLINKER_H
#define TARGET_HEXAGON_HEXAGONELFMCLINKER_H
#include <mcld/Target/ELFMCLinker.h>

namespace mcld {

class Module;
class FileHandle;

/** \class HexagonELFMCLinker
 *  \brief HexagonELFMCLinker sets up the environment for linking.
 *
 *  \see
 */
class HexagonELFMCLinker : public ELFMCLinker
{
public:
  HexagonELFMCLinker(LinkerConfig& pConfig,
                     mcld::Module& pModule,
                     FileHandle& pFileHandle);

  ~HexagonELFMCLinker();
};

} // namespace of mcld

#endif
