//===- SectionData.h ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LD_SECTION_DATA_H
#define MCLD_LD_SECTION_DATA_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/ADT/ilist.h>
#include <llvm/ADT/ilist_node.h>
#include <llvm/Support/DataTypes.h>

#include <mcld/LD/Fragment.h>

namespace mcld
{

class LDSection;

/** \class SectionData
 *  \brief SectionData provides a container for all Fragments.
 */
class SectionData
{
private:
  SectionData(const SectionData &);            // DO NOT IMPLEMENT
  SectionData& operator=(const SectionData &); // DO NOT IMPLEMENT

public:
  typedef llvm::iplist<Fragment> FragmentListType;
  typedef FragmentListType::iterator iterator;
  typedef FragmentListType::const_iterator const_iterator;

  typedef FragmentListType::reverse_iterator reverse_iterator;
  typedef FragmentListType::const_reverse_iterator const_reverse_iterator;

public:
  explicit SectionData(const LDSection &pSection);

  const LDSection &getSection() const { return *m_pSection; }

  unsigned int getAlignment() const { return m_Alignment; }
  void setAlignment(unsigned int pValue) { m_Alignment = pValue; }

  FragmentListType &getFragmentList() { return m_Fragments; }
  const FragmentListType &getFragmentList() const { return m_Fragments; }

  iterator begin() { return m_Fragments.begin(); }
  const_iterator begin() const { return m_Fragments.begin(); }

  iterator end() { return m_Fragments.end(); }
  const_iterator end() const { return m_Fragments.end(); }

  reverse_iterator rbegin() { return m_Fragments.rbegin(); }
  const_reverse_iterator rbegin() const { return m_Fragments.rbegin(); }

  reverse_iterator rend() { return m_Fragments.rend(); }
  const_reverse_iterator rend() const { return m_Fragments.rend(); }

  size_t size() const { return m_Fragments.size(); }

  bool empty() const { return m_Fragments.empty(); }

private:
  FragmentListType m_Fragments;
  const LDSection* m_pSection;
  unsigned int m_Alignment;

};

} // namespace of mcld

#endif

