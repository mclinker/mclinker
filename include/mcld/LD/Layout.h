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
#include <map>

namespace mcld
{
class MCLinker;
class LDSection;
class LDContext;
class TargetLDBackend;

/** \class Layout
 *  \brief Layout records the order and offset of all regions
 */
class Layout
{
public:
  typedef std::vector<LDSection*> SectionOrder;
  typedef SectionOrder::iterator sect_iterator;
  typedef SectionOrder::const_iterator const_sect_iterator;

private:
  struct Range
  {
    const LDSection* header;
    llvm::MCFragment* prevRear;
  };

  typedef std::vector<Range> RangeList;

  typedef std::map<const llvm::MCSectionData*, RangeList*> InputRangeList;

public:
  Layout();
  ~Layout();

  /// layoutFragment - perform layout for single fragment
  /// Assuming the previous fragment has already been laid out correctly.
  void layoutFragment(llvm::MCFragment& pFrag);

  /// getFragmentRefOffset - Get the offset of the given fragment inside its
  /// containing section.
  uint64_t getFragmentRefOffset(const MCFragmentRef& pFragRef) const;

  /// getFragmentOffset - Get the offset of the given fragment inside its
  /// containing section.
  uint64_t getFragmentOffset(const llvm::MCFragment& pFrag) const;

  /// getFragmentRef - give a LDSection in input file and an offset, return
  /// the fragment reference.
  MCFragmentRef getFragmentRef(const LDSection& pInputSection,
                               uint64_t pOffset) const;

  /// getInputLDSection - give a MCFragment, return the corresponding input
  /// LDSection*
  const LDSection* getInputLDSection(const llvm::MCFragment& pFrag) const;

  /// getInputLDSection - give a MCFragment, return the corresponding input
  /// LDSection*
  const LDSection* getOutputLDSection(const llvm::MCFragment& pFrag) const;

  /// \brief Invalidate all following fragments because a fragment has been
  /// resized. The fragments size should have already been updated.
  void invalidate(llvm::MCFragment& pFrag);

  /// numOfSections - the number of sections
  size_t numOfSections() const;

  /// numOfSegments - the number of segments
  size_t numOfSegments() const;

  // -----  modifiers  ----- //
  bool layout(LDContext& pOutput, const TargetLDBackend& pBackend);

  void addInputRange(const llvm::MCSectionData& pSD, const LDSection& pInputHdr);

  // -----  iterators  ----- //
  sect_iterator sect_begin()
  { return m_SectionOrder.begin(); }

  sect_iterator sect_end()
  { return m_SectionOrder.end(); }

  const_sect_iterator sect_begin() const
  { return m_SectionOrder.begin(); }

  const_sect_iterator sect_end() const
  { return m_SectionOrder.end(); }

private:
  /// \brief Make sure that the layout for the given fragment is valid, lazily
  /// computing it if necessary.
  void ensureValid(const llvm::MCFragment& pFrag) const;

  bool isFragmentUpToDate(const llvm::MCFragment& pFrag) const;

  /// orderRange - set fragment layout order in a range
  /// Assuming the last fragment in previous range is set
  void orderRange(llvm::MCFragment* pFront, llvm::MCFragment* pRear);

  /// ensureFragmentOrdered - Make sure that the layout order of a range that
  /// covers the given fragment is ordered, lazily setting the fragment order
  /// if needed
  void ensureFragmentOrdered(const llvm::MCFragment& pFrag) const;

  /// sortSectionOrder - perform sorting on m_SectionOrder to get final layout
  /// ordering
  void sortSectionOrder(const TargetLDBackend& pBackend);

private:
  /// a vector to describe the order of sections
  SectionOrder m_SectionOrder;

  InputRangeList m_InputRangeList;

  /// The last fragment which was laid out, or 0 if nothing has been laid
  /// out. Fragments are always laid out in order, so all fragments with a
  /// lower ordinal will be up to date.
  mutable llvm::DenseMap<const llvm::MCSectionData*, llvm::MCFragment*> m_LastValidFragment;

};

} // namespace of mcld

#endif

