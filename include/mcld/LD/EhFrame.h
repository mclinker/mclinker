//===- EhFrame.h ----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LD_EH_FRAME_H
#define MCLD_LD_EH_FRAME_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <vector>
#include <mcld/Fragment/RegionFragment.h>
#include <mcld/LD/LDSection.h>

namespace mcld {

class Layout;

/** \class EhFrame
 *  \brief EhFrame represents .eh_frame section
 */
class EhFrame
{
public:
  /** \class CIE
   *  \brief Common Information Entry.
   *  The CIE structure refers to LSB Core Spec 4.1, chap.10.6. Exception Frames.
   */
  class CIE : public RegionFragment
  {
  public:
    CIE(MemoryRegion& pRegion);

    void setFDEEncode(uint8_t pEncode) { m_FDEEncode = pEncode; }
    uint8_t getFDEEncode() const { return m_FDEEncode; }

  private:
    uint8_t m_FDEEncode;
  };

  /** \class FDE
   *  \brief Frame Description Entry
   *  The FDE structure refers to LSB Core Spec 4.1, chap.10.6. Exception Frames.
   */
  class FDE : public RegionFragment
  {
  public:
    FDE(MemoryRegion& pRegion,
        const CIE& pCIE,
        uint32_t pDataStart);

    const CIE& getCIE() const { return m_CIE; }

    uint32_t getDataStart() const { return m_DataStart; }
  private:
    const CIE& m_CIE;
    uint32_t m_DataStart;
  };

  typedef std::vector<CIE*> CIEList;
  typedef CIEList::iterator cie_iterator;
  typedef CIEList::const_iterator const_cie_iterator;

  typedef std::vector<FDE*> FDEList;
  typedef FDEList::iterator fde_iterator;
  typedef FDEList::const_iterator const_fde_iterator;

public:
  EhFrame(LDSection& pSection, Layout& pLayout);

  ~EhFrame();

  const LDSection& getSection() const { return m_Section; }
  LDSection&       getSection()       { return m_Section; }

  // -----  fragment  ----- //
  /// addFragment - when we start treating CIEs and FDEs as regular fragments,
  /// we call this function instead of addCIE() and addFDE().
  /// @param pSection - input .eh_frame section.
  void addFragment(RegionFragment& pFrag, LDSection& pSection);

  // -----  CIE  ----- //
  const_cie_iterator cie_begin() const { return m_CIEs.begin(); }
  cie_iterator       cie_begin()       { return m_CIEs.begin(); }
  const_cie_iterator cie_end  () const { return m_CIEs.end(); }
  cie_iterator       cie_end  ()       { return m_CIEs.end(); }

  const CIE& cie_front() const { return *m_CIEs.front(); }
  CIE&       cie_front()       { return *m_CIEs.front(); }
  const CIE& cie_back () const { return *m_CIEs.back(); }
  CIE&       cie_back ()       { return *m_CIEs.back(); }

  void addCIE(CIE& pCIE);

  /// numOfCIEs - return the number of CIE entries
  size_t numOfCIEs() const { return m_CIEs.size(); }

  // -----  FDE  ----- //
  const_fde_iterator fde_begin() const { return m_FDEs.begin(); }
  fde_iterator       fde_begin()       { return m_FDEs.begin(); }
  const_fde_iterator fde_end  () const { return m_FDEs.end(); }
  fde_iterator       fde_end  ()       { return m_FDEs.end(); }

  const FDE& fde_front() const { return *m_FDEs.front(); }
  FDE&       fde_front()       { return *m_FDEs.front(); }
  const FDE& fde_back () const { return *m_FDEs.back(); }
  FDE&       fde_back ()       { return *m_FDEs.back(); }

  void addFDE(FDE& pFDE);

  /// numOfFDEs - the number of FDE entries
  size_t numOfFDEs() const { return m_FDEs.size(); }

private:
  LDSection& m_Section;
  CIEList m_CIEs;
  FDEList m_FDEs;
  Layout& m_Layout;
};

} // namespace of mcld

#endif

