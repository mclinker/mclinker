//===- EhFrame.h ----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_EXCEPTION_HANDLING_FRAME_H
#define MCLD_EXCEPTION_HANDLING_FRAME_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <vector>

namespace mcld
{

class FDE;
class CIE;
/** \class EhFrame
 *  \brief EhFrame represents .eh_frame section
 */
class EhFrame
{
public:
  EhFrame();
  ~EhFrame();

private:
  std::vector<FDE*> m_FDEs;
  std::vector<CIE*> m_CIEs;
};

} // namespace of mcld

#endif

