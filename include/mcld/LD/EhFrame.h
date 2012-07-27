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
#include <mcld/LD/RegionFragment.h>
#include <mcld/Support/GCFactory.h>

namespace mcld
{

class Input;
class Layout;
class SectionData;
class TargetLDBackend;

/** \class EhFrame
 *  \brief EhFrame represents .eh_frame section
 *  EhFrame is responsible to parse the input eh_frame sections and create
 *  the corresponding CIE and FDE entries.
 */
class EhFrame
{
public:
  typedef ConstTraits<unsigned char>::pointer ConstAddress;
  typedef std::vector<CIE*> CIEListType;
  typedef std::vector<FDE*> FDEListType;
  typedef CIEListType::iterator cie_iterator;
  typedef CIEListType::const_iterator const_cie_iterator;
  typedef FDEListType::iterator fde_iterator;
  typedef FDEListType::const_iterator const_fde_iterator;

public:
  EhFrame();
  ~EhFrame();

  /// readEhFrame - read an .eh_frame section and create the corresponding
  /// CIEs and FDEs
  /// @param pSD - the SectionData of this input eh_frame
  /// @param pSection - the input eh_frame
  /// @param pArea - the memory area which pSection is within.
  /// @ return - size of this eh_frame section, 0 if we do not recognize
  /// this eh_frame or this is an empty section
  uint64_t readEhFrame(Layout& pLayout,
                       const TargetLDBackend& pBackend,
                       SectionData& pSD,
                       const Input& pInput,
                       LDSection& pSection,
                       MemoryArea& pArea);

  // ----- observers ----- //
  cie_iterator cie_begin()
  { return m_CIEs.begin(); }

  const_cie_iterator cie_begin() const
  { return m_CIEs.begin(); }

  cie_iterator cie_end()
  { return m_CIEs.end(); }

  const_cie_iterator cie_end() const
  { return m_CIEs.end(); }

  fde_iterator fde_begin()
  { return m_FDEs.begin(); }

  const_fde_iterator fde_begin() const
  { return m_FDEs.begin(); }

  fde_iterator fde_end()
  { return m_FDEs.end(); }

  const_fde_iterator fde_end() const
  { return m_FDEs.end(); }

  /// getFDECount - the number of FDE entries
  size_t getFDECount()
  { return m_FDEs.size(); }

  size_t getFDECount() const
  { return m_FDEs.size(); }

  /// canRecognizeAllEhFrame - return if we are able to parse all input
  /// eh_frame sections
  /// @return false - if there is any input .eh_frame section that
  /// we are not able to recognize
  bool canRecognizeAllEhFrame()
  { return m_fCanRecognizeAll; }

  bool canRecognizeAllEhFrame() const
  { return m_fCanRecognizeAll; }

private:
  typedef std::vector<Fragment*> FragListType;

private:
  /// addCIE - parse and create a CIE entry
  /// @return false - cannot recognize this CIE
  bool addCIE(MemoryRegion& pFrag,
              const TargetLDBackend& pBackend,
              FragListType& pFragList);

  /// addFDE - parse and create an FDE entry
  /// @return false - cannot recognize this FDE
  bool addFDE(MemoryRegion& pFrag,
              const TargetLDBackend& pBackend,
              FragListType& pFragList);

  /// readVal - read a 32 bit data from pAddr, swap it if needed
  uint32_t readVal(ConstAddress pAddr, bool pIsTargetLittleEndian);

  /// skipLEB128 - skip the first LEB128 encoded value from *pp, update *pp
  /// to the next character.
  /// @return - false if we ran off the end of the string.
  /// @ref - GNU gold 1.11, ehframe.h, Eh_frame::skip_leb128.
  bool skipLEB128(ConstAddress* pp, ConstAddress pend);

  /// deleteFragments - release the MemoryRegion and delete Fragments in pList
  void deleteFragments(FragListType& pList, MemoryArea& pArea);

private:
  CIEListType m_CIEs;
  FDEListType m_FDEs;

  bool m_fCanRecognizeAll;
};

} // namespace of mcld

#endif

