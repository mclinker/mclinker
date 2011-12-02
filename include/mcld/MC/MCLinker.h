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
#include "mcld/MC/MCLDInfo.h"
#include "llvm/ADT/StringRef.h"

namespace mcld {
class MCLDInfo;
class TargetLDBackend;

//===----------------------------------------------------------------------===//
/// MCLinker - MCLinker provides APIs of a linker.
///
class MCLinker
{
public:
  MCLinker(MCLDInfo& pLDInfo, TargetLDBackend& pLDBackend);
  ~MCLinker();

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
  explicit MCLinker(TargetLDBackend&, MCLDInfo&);
  ~MCLinker();

  void applyRelocations();

private:
  TargetLDBackend& m_pBackend;
  MCLDInfo& m_pInfo;
};

} // namespace of mcld

#endif

