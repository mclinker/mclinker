//===- Layout.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LD_LAYOUT_H
#define MCLD_LD_LAYOUT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <map>

#include <llvm/ADT/ilist.h>
#include <llvm/ADT/ilist_node.h>
#include <llvm/ADT/DenseMap.h>

#include <mcld/LD/FragmentRef.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/SectionData.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/Support/GCFactory.h>

namespace mcld
{
class MCLinker;
class Output;
class TargetLDBackend;

/** \class Layout
 *  \brief Layout maintains the mapping between sections and fragments.
 *
 *  MCLinker is a fragment-based linker. But readers and target backends
 *  still need section information. Layout is used to maintain the mapping
 *  between sections and fragments. Layout helps readers and target backends
 *  get the input or output section information from a fragment.
 */
class Layout
{
public:
  typedef std::vector<LDSection*> SectionOrder;
  typedef SectionOrder::iterator sect_iterator;
  typedef SectionOrder::const_iterator const_sect_iterator;

public:
  /// constructor
  Layout();

  /// destructor
  ~Layout();

  /// getInputLDSection - give a Fragment, return the corresponding input
  /// LDSection*
  ///
  /// @return return NULL if the fragment is not found in input
  LDSection* getInputLDSection(const Fragment& pFrag);

  /// getInputLDSection - give a Fragment, return the corresponding input
  /// LDSection*
  ///
  /// @return return NULL if the fragment is not found in input
  const LDSection* getInputLDSection(const Fragment& pFrag) const;

  /// getFragmentRef - give a LDSection in input file and an offset, return
  /// the fragment reference.
  ///
  /// @param pInputSection - the given input section
  /// @param pOffset - the offset, cannot be larger than this input section.
  /// @return if found, return the fragment. Otherwise, return NULL.
  FragmentRef*
  getFragmentRef(const LDSection& pInputSection, uint64_t pOffset);

  /// getFragmentRef - give a fragment and a big offset, return the fragment
  /// reference in the section data.
  ///
  /// @param pFrag - the given fragment
  /// @param pBigOffset - the offset, can be larger than the fragment, but can
  ///                     not larger than this input section.
  /// @return if found, return the fragment. Otherwise, return NULL.
  FragmentRef* getFragmentRef(const Fragment& pFrag, uint64_t pBigOffset);

  /// getOutputOffset - Get the offset of the given fragment inside the
  /// the output's SectionData.
  uint64_t getOutputOffset(const Fragment& pFrag);

  /// getOutputOffset - Get the offset of the given fragment inside the
  /// the output's SectionData.
  uint64_t getOutputOffset(const Fragment& pFrag) const;

  /// getOutputOffset - Get the offset of the given fragment inside
  /// the output's SectionData.
  ///
  /// @return return -1 if the fragment is not found in output's SectionData.

  uint64_t getOutputOffset(const FragmentRef& pFragRef);
  /// getOutputOffset - Get the offset of the given fragment inside
  /// the output's SectionData.
  ///
  /// @return return -1 if the fragment is not found in output's SectionData.
  uint64_t getOutputOffset(const FragmentRef& pFragRef) const;

  /// getOutputLDSection - give a Fragment, return the corresponding output
  /// LDSection*
  ///
  /// @return return NULL if the fragment is not found in the output
  LDSection* getOutputLDSection(const Fragment& pFrag);

  /// getOutputLDSection - give a Fragment, return the corresponding output
  /// LDSection*
  ///
  /// @return return NULL if the fragment is not found in the output
  const LDSection* getOutputLDSection(const Fragment& pFrag) const;

  // -----  modifiers  ----- //
  bool layout(Output& pOutput,
              const TargetLDBackend& pBackend, const MCLDInfo& pInfo);

  /// addInputRange
  void addInputRange(const SectionData& pSD, const LDSection& pInputHdr);

  /// appendFragment - append the given Fragment to the given SectionData,
  /// and insert a AlignFragment to preserve the required align constraint if
  /// needed
  /// @return return the inserted size, i.e., the size of pFrag and alignment
  /// size if any
  uint64_t appendFragment(Fragment& pFrag, SectionData& pSD,
                          uint32_t pAlignConstraint = 1);
private:
  /** \class Range
   *  \brief Range is a <input's LDSection, previous rear fragment> pair
   */
  struct Range : public llvm::ilist_node<Range>
  {
  public:
    Range();
    Range(const LDSection& pHeader);
    ~Range();

  public:
    LDSection* header;
    Fragment* prevRear;
  };

  typedef llvm::iplist<Range> RangeList;

  typedef std::map<const SectionData*, RangeList*> SDRangeMap;

  typedef GCFactory<FragmentRef, 0> FragRefFactory;

private:
  inline bool isFirstRange(const Range& pRange) const
  { return (NULL == pRange.prevRear); }

  inline bool isLastRange(const Range& pRange) const
  { return (NULL == pRange.getNextNode()); }

  inline bool isEmptyRange(const Range& pRange) const
  {
    if (isFirstRange(pRange)) {
      if (!pRange.header->hasSectionData() ||
          pRange.header->getSectionData()->getFragmentList().empty())
        return true;
      else
        return false;
    }
    return (NULL == pRange.prevRear->getNextNode());
  }

  // get the front fragment in the range.
  inline Fragment* getFront(Range& pRange) const
  {
    if (!pRange.header->hasSectionData())
      return NULL;
    if (pRange.header->getSectionData()->getFragmentList().empty())
      return NULL;

    if (isFirstRange(pRange))
      return &pRange.header->getSectionData()->getFragmentList().front();

    if (isEmptyRange(pRange))
      return NULL;

    return pRange.prevRear->getNextNode();
  }

  inline const Fragment* getFront(const Range& pRange) const
  {
    if (!pRange.header->hasSectionData())
      return NULL;
    if (pRange.header->getSectionData()->getFragmentList().empty())
      return NULL;

    if (isFirstRange(pRange))
      return &pRange.header->getSectionData()->getFragmentList().front();

    if (isEmptyRange(pRange))
      return NULL;

    return pRange.prevRear->getNextNode();
  }

  // get the rear fragment in the range.
  inline Fragment* getRear(Range& pRange) const
  {
    if (!pRange.header->hasSectionData())
      return NULL;
    if (pRange.header->getSectionData()->getFragmentList().empty())
      return NULL;

    if (isLastRange(pRange)) {
      if (isEmptyRange(pRange))
        return NULL;
      return &pRange.header->getSectionData()->getFragmentList().back();
    }
    return pRange.getNextNode()->prevRear;
  }

  inline const Fragment* getRear(const Range& pRange) const
  {
    if (!pRange.header->hasSectionData())
      return NULL;
    if (pRange.header->getSectionData()->getFragmentList().empty())
      return NULL;

    if (isLastRange(pRange)) {
      if (isEmptyRange(pRange))
        return NULL;
      return &pRange.header->getSectionData()->getFragmentList().back();
    }
    return pRange.getNextNode()->prevRear;
  }

  FragmentRef* getFragmentRef(Range &pRange, uint64_t pOffset);

  FragmentRef*
  getFragmentRef(Fragment& pFront, Fragment& pRear, uint64_t pOffset);

  bool hasLayoutOrder(const Fragment& pFragment) const
  { return (pFragment.getLayoutOrder() != ~(0U)); }

  bool hasLayoutOffset(const Fragment& pFragment) const
  { return (pFragment.getOffset() != ~UINT64_C(0)); }

  bool isValidOffset(const Fragment& pFrag, uint64_t pTargetOffset) const;

  void setFragmentLayoutOrder(Fragment* pFragment);

  void setFragmentLayoutOffset(Fragment* pFragment);

  /// sortSectionOrder - perform sorting on m_SectionOrder to get final layout
  /// ordering
  void sortSectionOrder(const Output& pOutput,
                        const TargetLDBackend& pBackend,
                        const MCLDInfo& pInfo);

private:
  /// a vector to describe the order of sections
  SectionOrder m_SectionOrder;

  /// the map from SectionData* to its own RangeList.
  SDRangeMap m_SDRangeMap;

  FragRefFactory m_FragRefFactory;
};

} // namespace of mcld

#endif

