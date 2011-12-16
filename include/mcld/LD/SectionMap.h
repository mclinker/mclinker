//===- SectionMap.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef SECTIONMAP_H
#define SECTIONMAP_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <vector>
#include <string>

namespace mcld
{

/** \class SectionMap
 *  \brief descirbe the mappings of input section's name (or prefix) to
 *         its associated output section's name
 */
class SectionMap
{
public:
  typedef std::pair<std::string, std::string> NamePairTy;
  typedef std::vector<NamePairTy> NameMapTy;

  typedef NameMapTy::iterator iterator;
  typedef NameMapTy::const_iterator const_iterator;

public:
  SectionMap();
  ~SectionMap();

  // add a mapping from input name (maybe prefix) to output name.
  // return true if succeeding to add one, and return false if failing.
  bool addNameMapping(const std::string& pFrom, const std::string& pTo);

  // return the corresponding name of output section from the given input
  const std::string& getOutputSectionName(const std::string& pFrom);

  // -----  iterators  ----- //
  iterator begin()
  { return m_NameMap.begin(); }

  iterator end()
  { return m_NameMap.end(); }

  const_iterator begin() const
  { return m_NameMap.begin(); }

  const_iterator end() const
  { return m_NameMap.end(); }

private:
  NameMapTy m_NameMap;
};

} // namespace of mcld

#endif

