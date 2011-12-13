//===- MCLinker.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides a number of APIs used by SectLinker.
// These APIs do the things which a linker should do.
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_MCLINKER_H
#define MCLD_MCLINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/MC/MCLDCommand.h"

namespace mcld {

class MCLDCommand;
class MCLDFile;
class Layout;

class TargetLDBackend;
class MCLDInfo;

/** \class MCLinker
 *  \brief MCLinker provides a pass to link object files.
 */
class MCLinker
{
public:
  explicit MCLinker(TargetLDBackend& pBackend, MCLDInfo& pLDInfo);
  ~MCLinker();

private:
  TargetLDBackend& m_Backend;
  MCLDInfo& m_Info;
};

} // namespace of mcld

#endif

