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

#include <vector>
#include <map>

#include <llvm/ADT/ilist.h>
#include <llvm/ADT/ilist_node.h>
#include <llvm/ADT/DenseMap.h>

#include <mcld/LD/LDSection.h>
#include <mcld/LD/SectionData.h>
#include <mcld/LinkerConfig.h>
#include <mcld/Support/GCFactory.h>

namespace mcld {

class Module;
class FragmentRef;
class FragmentLinker;
class TargetLDBackend;

/** \class Layout
 *  \brief Layout maintains the mapping between sections and fragments.
 *
 *  FragmentLinker is a fragment-based linker. But readers and target backends
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

  // -----  modifiers  ----- //
  bool layout(Module& pModule,
              const TargetLDBackend& pBackend,
              const LinkerConfig& pConfig);

  /// addInputRange
  void addInputRange(const SectionData& pSD, const LDSection& pInputHdr);

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

  /// sortSectionOrder - perform sorting on m_SectionOrder to get final layout
  /// ordering
  void sortSectionOrder(const TargetLDBackend& pBackend,
                        const LinkerConfig& pConfig);

private:
  /// a vector to describe the order of sections
  SectionOrder m_SectionOrder;

  /// the map from SectionData* to its own RangeList.
  SDRangeMap m_SDRangeMap;
};

} // namespace of mcld

#endif

