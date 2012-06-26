//===- Linker.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ALONE_SUPPORT_LINKER_CONFIG_H
#define ALONE_SUPPORT_LINKER_CONFIG_H

#include <string>

namespace alone {

class LinkerConfig {
public:
  LinkerConfig(const std::string& pTriple);

  virtual ~LinkerConfig() { }
};

} // end namespace alone

#endif // ALONE_SUPPORT_LINKER_CONFIG_H
