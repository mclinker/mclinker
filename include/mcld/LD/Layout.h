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
  // -----  modifiers  ----- //
  bool layout(Module& pModule,
              const TargetLDBackend& pBackend,
              const LinkerConfig& pConfig);

private:
  /// sortSectionOrder - perform sorting on m_SectionOrder to get final layout
  /// ordering
  void sortSectionOrder(const TargetLDBackend& pBackend,
                        const LinkerConfig& pConfig);

private:
  typedef std::vector<LDSection*> SectionOrder;

private:
  /// a vector to describe the order of sections
  SectionOrder m_SectionOrder;
};

} // namespace of mcld

#endif

