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
#include <mcld/ADT/SizeTraits.h>
#include <mcld/LD/CIE.h>
#include <mcld/LD/FDE.h>

namespace mcld
{

class Input;
class Layout;
class LDSection;
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
  EhFrame(bool isLittleEndianTarget);
  ~EhFrame();

  /// read - read an .eh_frame section and create the corresponding
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

  /// getFDECount - the number of FDE entries
  size_t getFDECount() const
  { return m_FDEs.size(); }

  /// treatAsRegularSection - if we should treat eh_frame as regular
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

  /// State - the return state of an Action. It is used to determine the next
  /// action function. Each State corresponds to an action function except
  /// 'Stop' state
  enum State {
    Start         = 0,
    CheckFirstCIE = 1,
    ParseEntry    = 2,
    AddCIE        = 3,
    AddFDE        = 4,
    Fail          = 5,
    Success       = 6,
    ReadRegular   = 7,
    Stop
  };

  /// Package - collect all needed data used by every actions so that they can
  /// share this data
  struct Package {
    Package(Layout& pLayout,
            Input& pInput,
            LDSection& pSection,
            unsigned int pBitclass)
      : layout(pLayout),
        input(pInput),
        section(pSection),
        bitclass(pBitclass),
        sectCur(0x0),
        sectStart(0x0),
        sectEnd(0x0),
        sectionRegion(NULL),
        entryRegion(NULL),
        sectSize(0x0) {
    }

    Layout& layout;
    Input& input;
    LDSection& section;
    unsigned int bitclass;
    ConstAddress sectCur;
    ConstAddress sectStart;
    ConstAddress sectEnd;
    FragListType fragList;
    MemoryRegion* sectionRegion;
    MemoryRegion* entryRegion;
    size_t sectSize;
  };
  typedef struct Package PackageType;

  /** \class ReadValIF
   *  \brief ReadValIF provides interface for ReadVal, which is used to read
   *   32 bit value from given context, see @ref m_pReadVal
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
  // ----- action functions ----- //
  State start(PackageType& pPkg);
  State checkFirstCIE(PackageType& pPkg);
  State parseEntry(PackageType& pPkg);
  State addCIE(PackageType& pPkg);
  State addFDE(PackageType& pPkg);
  State fail(PackageType& pPkg);
  State success(PackageType& pPkg);
  State readRegular(PackageType& pPkg);

  /// skipLEB128 - skip the first LEB128 encoded value from *pp, update *pp
  /// to the next character.
  /// @return - false if we ran off the end of the string.
  /// @ref - GNU gold 1.11, ehframe.h, Eh_frame::skip_leb128.
  bool skipLEB128(ConstAddress* pp, ConstAddress pend);

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

