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

#include <mcld/ADT/TypeTraits.h>
#include <mcld/LD/CIE.h>
#include <mcld/LD/FDE.h>
#include <mcld/MC/MCRegionFragment.h>
#include <mcld/Support/GCFactory.h>

namespace mcld
{
/** \class EhFrame
 *  \brief EhFrame represents .eh_frame section
 */
class EhFrame
{
public:
  typedef ConstTraits<unsigned char>::pointer ConstAddress;

public:
  EhFrame();
  ~EhFrame();

  // readEhFrame - read an .eh_frame section and create the corresponding
  // CIEs and FDEs
  bool readEhFrame(MCLinker* pLinker,
                   const LDSection& pSection,
                   MemoryArea& pArea);

private:
  bool readCIE(const MCRegionFragment& pFrag, bool pIsTargetLittleEndian);
  bool readFDE(const MCRegionFragment& pFrag, bool pIsTargetLittleEndian);

  // readVal - read a 32 bit data from pAddr, swap if needed
  uint32_t readVal(ConstAddress pAddr, bool pIsTargetLittleEndian);

private:
  typedef GCFactory<CIE, 0> CIEFactory;
  typedef GCFactory<FDE, 0> FDEFactory;

private:
  CIEFactory m_CIEFactory;
  FDEFactory m_FDEFactory;
  std::vector<FDE*> m_FDEs;
  std::vector<CIE*> m_CIEs;
};

} // namespace of mcld

#endif

