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

#include <llvm/Support/Host.h>
#include <mcld/ADT/TypeTraits.h>
#include <mcld/ADT/SizeTraits.h>
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
  EhFrame(bool isTargetLittleEndian);
  ~EhFrame();

  /// readEhFrame - read an .eh_frame section and create the corresponding
  /// CIEs and FDEs
  /// @param pInput - the Input contains this eh_frame
  /// @param pSection - the input eh_frame
  /// @return - size of this eh_frame section
  size_t read(Layout& pLayout,
              Input& pInput,
              LDSection& pSection,
              unsigned int pBitclass);

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

  /// shouldTreatAsRegularSection - if we should treat eh_frame as regular
  /// sections
  /// @return true - there is any input .eh_frame section that we are not
  /// able to recognize and should treat the eh_frame sections as regular
  /// sections
  bool treatAsRegularSection()
  { return m_fTreatAsRegularSection; }

  bool treatAsRegularSection() const
  { return m_fTreatAsRegularSection; }

private:
  typedef std::vector<Fragment*> FragListType;

  enum Result {
    None,
    Success,
    Fail,
    Terminator
  };

private:
  Result parse(Layout& pLayout,
               Input& pInput,
               LDSection& pSection,
               unsigned int pBitclass,
               size_t& pSize);

  /// addCIE - parse and create a CIE entry
  /// @return false - cannot recognize this CIE
  bool addCIE(MemoryRegion& pFrag,
              FragListType& pFragList,
              unsigned int pBitclass);

  /// addFDE - parse and create an FDE entry
  /// @return false - cannot recognize this FDE
  bool addFDE(MemoryRegion& pFrag,
              FragListType& pFragList);

  /// skipLEB128 - skip the first LEB128 encoded value from *pp, update *pp
  /// to the next character.
  /// @return - false if we ran off the end of the string.
  /// @ref - GNU gold 1.11, ehframe.h, Eh_frame::skip_leb128.
  bool skipLEB128(ConstAddress* pp, ConstAddress pend);

  /// deleteFragments - release the MemoryRegion and delete Fragments in pList
  void deleteFragments(FragListType& pList, MemoryArea& pArea);

  /** \class ReadValIF
   *  \brief ReadValIF provides interface for ReadVal
   */
  class ReadValIF
  {
  public:
    virtual ~ReadValIF() {}
    virtual uint32_t operator()(ConstAddress pAddr) = 0;
  };

  /** \class ReadVal
   *  \brief ReadVal is a template scaffolding for partial specification
   */
  template<bool NEEDSWAP>
  class ReadVal
  { };

private:
  CIEListType m_CIEs;
  FDEListType m_FDEs;

  bool m_fTreatAsRegularSection;

  /// m_pReadVal - a functor of ReadVal
  /// use (*m_pReadVal)(pAddr) to read a 32 bit data from pAddr
  ReadValIF* m_pReadVal;
};

/** \class ReadVal<true>
 *  \brief ReadVal<true> provides operator to read 32 bit value from the
 *   given pointer and swap it
 */
template<>
class EhFrame::ReadVal<true> : public ReadValIF
{
public:
  ~ReadVal() {}
  uint32_t operator()(ConstAddress pAddr) {
    const uint32_t* p = reinterpret_cast<const uint32_t*>(pAddr);
    uint32_t val = *p;
    // need swap because the host and target have different endian
    val = bswap32(val);
    return val;
  }
};

/** \class ReadVal<false>
 *  \brief ReadVal<false> provides operator to read 32 bit value from the
 *   given pointer
 */
template<>
class EhFrame::ReadVal<false> : public ReadValIF
{
public:
  ~ReadVal() {}
  uint32_t operator()(ConstAddress pAddr) {
    const uint32_t* p = reinterpret_cast<const uint32_t*>(pAddr);
    uint32_t val = *p;
    return val;
  }
};

} // namespace of mcld

#endif

