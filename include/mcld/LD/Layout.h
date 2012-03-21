//===- Layout.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LAYOUT_H
#define MCLD_LAYOUT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/ADT/ilist.h>
#include <llvm/ADT/ilist_node.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/MC/MCAssembler.h>
#include <mcld/MC/MCFragmentRef.h>
#include <mcld/Support/GCFactory.h>
#include <mcld/LD/LDSection.h>
#include <mcld/MC/MCLDInfo.h>
#include <map>

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

  /// getInputLDSection - give a MCFragment, return the corresponding input
  /// LDSection*
  ///
  /// @return return NULL if the fragment is not found in input
  LDSection* getInputLDSection(const llvm::MCFragment& pFrag);

  /// getInputLDSection - give a MCFragment, return the corresponding input
  /// LDSection*
  ///
  /// @return return NULL if the fragment is not found in input
  const LDSection* getInputLDSection(const llvm::MCFragment& pFrag) const;

  /// getFragmentRef - give a LDSection in input file and an offset, return
  /// the fragment reference.
  ///
  /// @param pInputSection - the given input section
  /// @param pOffset - the offset, cannot be larger than this input section.
  /// @return if found, return the fragment. Otherwise, return NULL.
  MCFragmentRef*
  getFragmentRef(const LDSection& pInputSection, uint64_t pOffset);

  /// getFragmentRef - give a fragment and a big offset, return the fragment
  /// reference in the section data.
  ///
  /// @param pFrag - the given fragment
  /// @param pBigOffset - the offset, can be larger than the fragment, but can
  ///                     not larger than this input section.
  /// @return if found, return the fragment. Otherwise, return NULL.
  MCFragmentRef*
  getFragmentRef(const llvm::MCFragment& pFrag, uint64_t pBigOffset);

  /// getOutputOffset - Get the offset of the given fragment inside the
  /// the output's MCSectionData.
  uint64_t getOutputOffset(const llvm::MCFragment& pFrag);

  /// getOutputOffset - Get the offset of the given fragment inside the
  /// the output's MCSectionData.
  uint64_t getOutputOffset(const llvm::MCFragment& pFrag) const;

  /// getOutputOffset - Get the offset of the given fragment inside
  /// the output's MCSectionData.
  ///
  /// @return return -1 if the fragment is not found in output's MCSectionData.

  uint64_t getOutputOffset(const MCFragmentRef& pFragRef);
  /// getOutputOffset - Get the offset of the given fragment inside
  /// the output's MCSectionData.
  ///
  /// @return return -1 if the fragment is not found in output's MCSectionData.
  uint64_t getOutputOffset(const MCFragmentRef& pFragRef) const;

  /// getOutputLDSection - give a MCFragment, return the corresponding output
  /// LDSection*
  ///
  /// @return return NULL if the fragment is not found in the output
  LDSection* getOutputLDSection(const llvm::MCFragment& pFrag);

  /// getOutputLDSection - give a MCFragment, return the corresponding output
  /// LDSection*
  ///
  /// @return return NULL if the fragment is not found in the output
  const LDSection* getOutputLDSection(const llvm::MCFragment& pFrag) const;

  // -----  modifiers  ----- //
  bool layout(Output& pOutput, const TargetLDBackend& pBackend, const MCLDInfo& pInfo);

  /// addInputRange
  void addInputRange(const llvm::MCSectionData& pSD,
                     const LDSection& pInputHdr);

  /// appendFragment - append the given MCFragment to the given MCSectionData,
  /// and insert a MCAlignFragment to preserve the required align constraint if
  /// needed
  /// @return return the inserted size, i.e., the size of pFrag and alignment
  /// size if any
  uint64_t appendFragment(llvm::MCFragment& pFrag,
                          llvm::MCSectionData& pSD,
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
    llvm::MCFragment* prevRear;
  };

  typedef llvm::iplist<Range> RangeList;

  typedef std::map<const llvm::MCSectionData*, RangeList*> SDRangeMap;

  typedef GCFactory<MCFragmentRef, 0> FragRefFactory;

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
  inline llvm::MCFragment* getFront(Range& pRange) const
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

  inline const llvm::MCFragment* getFront(const Range& pRange) const
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
  inline llvm::MCFragment* getRear(Range& pRange) const
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

  inline const llvm::MCFragment* getRear(const Range& pRange) const
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

  MCFragmentRef* getFragmentRef(Range &pRange, uint64_t pOffset);

  MCFragmentRef* getFragmentRef(llvm::MCFragment& pFront,
                                llvm::MCFragment& pRear,
                                uint64_t pOffset);

  bool hasLayoutOrder(const llvm::MCFragment& pFragment) const
  { return (pFragment.getLayoutOrder() != ~(0U)); }

  bool hasLayoutOffset(const llvm::MCFragment& pFragment) const
  { return (pFragment.Offset != ~UINT64_C(0)); }

  bool isValidOffset(const llvm::MCFragment& pFrag, uint64_t pTargetOffset) const;

  void setFragmentLayoutOrder(llvm::MCFragment* pFragment);

  void setFragmentLayoutOffset(llvm::MCFragment* pFragment);

  /// sortSectionOrder - perform sorting on m_SectionOrder to get final layout
  /// ordering
  void sortSectionOrder(const Output& pOutput,
                        const TargetLDBackend& pBackend,
                        const MCLDInfo& pInfo);

private:
  /// a vector to describe the order of sections
  SectionOrder m_SectionOrder;

  /// the map from MCSectionData* to its own RangeList.
  SDRangeMap m_SDRangeMap;

  FragRefFactory m_FragRefFactory;
};

} // namespace of mcld

#endif

