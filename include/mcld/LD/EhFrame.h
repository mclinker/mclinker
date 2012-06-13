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
 *  EhFrame is responsible to parse the input eh_frame sections and create
 *  the corresponding CIE and FDE entries.
 */

class TargetLDBackend;

class EhFrame
{
public:
  typedef ConstTraits<unsigned char>::pointer ConstAddress;
  typedef std::vector<CIE*>::iterator cie_iterator;
  typedef std::vector<CIE*>::const_iterator cie_const_iterator;
  typedef std::vector<FDE*>::iterator fde_iterator;
  typedef std::vector<FDE*>::const_iterator fde_const_iterator;

public:
  EhFrame();
  ~EhFrame();

  /// readEhFrame - read an .eh_frame section and create the corresponding
  /// CIEs and FDEs
  /// @param pSection - the input section
  /// @param pArea - the memory area which pSection is within.
  /// @ return - size of this eh_frame section, 0 if we do not recognize
  /// this eh_frame or this is an empty section
  uint64_t readEhFrame(MCLinker* pLinker,
                       const TargetLDBackend& pBackend,
                       LDSection& pSection,
                       MemoryArea& pArea);

private:
  /// addCIE - parse and create a CIE entry
  /// @return false - cannot recognize this CIE
  bool addCIE(const MCRegionFragment& pFrag, const TargetLDBackend& pBackend);

  /// addFDE - parse and create an FDE entry
  /// @return false - cannot recognize this FDE
  bool addFDE(const MCRegionFragment& pFrag, const TargetLDBackend& pBackend);

  /// readVal - read a 32 bit data from pAddr, swap it if needed
  uint32_t readVal(ConstAddress pAddr, bool pIsTargetLittleEndian);

  /// skipLEB128 - skip the first LEB128 encoded value from *pp, update *pp
  /// to the next character.
  /// @return - false if we ran off the end of the string.
  /// @ref - GNU gold 1.11, ehframe.h, Eh_frame::skip_leb128.
  bool skipLEB128(ConstAddress* pp, ConstAddress pend);

private:
  typedef GCFactory<CIE, 0> CIEFactory;
  typedef GCFactory<FDE, 0> FDEFactory;

private:
  CIEFactory m_CIEFactory;
  FDEFactory m_FDEFactory;
  std::vector<CIE*> m_CIEs;
  std::vector<FDE*> m_FDEs;
};

} // namespace of mcld

#endif

