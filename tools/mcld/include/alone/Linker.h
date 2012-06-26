//===- Linker.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ALONE_LINKER_H
#define ALONE_LINKER_H

namespace alone {

class LinkerConfig;

class Linker {
private:
  LinkerConfig *mLinkerConfig;

public:
  Linker();

  Linker(const LinkerConfig& pConfig);

  ~Linker();

  Linker& setConfig(const LinkerConfig& pConfig);

  LinkerConfig* config()
  { return mLinkerConfig; }

  const LinkerConfig* config() const
  { return mLinkerConfig; }

};

} // end namespace alone

#endif // ALONE_LINKER_H
