/*****************************************************************************
 *   The mcld Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@mediatek.com>                                         *
 *   Luba Tang <luba.tang@mediatek.com>                                      *
 ****************************************************************************/
#ifndef MCLDDRIVER_H
#define MCLDDRIVER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld {
class MCLDInfo;
class TargetLDBackend;

//===----------------------------------------------------------------------===//
/// MCLDDriver - MCLDDriver prepares parameters for MCLinker.
///
class MCLDDriver 
{
public:
  MCLDDriver(const MCLDInfo& pInfo, TargetLDBackend& pLDBackend);
  ~MCLDDriver();

  void addInputFile(StringRef pFilename);
  void enterGroup();
  void leaveGroup();
private:
  TargetLDBackend &m_LDBackend;
};

} //end namespace mcld 

#endif

