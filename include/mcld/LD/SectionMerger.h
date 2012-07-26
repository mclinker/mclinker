//===- SectionMerger.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SECTION_MERGER_H
#define MCLD_SECTION_MERGER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <vector>
#include <string>

#include <mcld/LD/LDSection.h>
#include <mcld/LD/LDContext.h>
#include <mcld/LD/SectionMap.h>

namespace mcld
{
class MCLinker;

/** \class SectionMerger
 *  \brief maintain the mappings of substr of input section name to associated
 *         output section (data)
 */
class SectionMerger
{
public:
  struct Mapping {
    std::string inputSubStr;
    LDSection* outputSection;
  };
  typedef std::vector<Mapping> LDSectionMapTy;

  typedef LDSectionMapTy::iterator iterator;
  typedef LDSectionMapTy::const_iterator const_iterator;

public:
  SectionMerger(SectionMap& pSectionMap, LDContext& pContext);
  ~SectionMerger();

  /// getOutputSectHdr - return a associated output section header
  LDSection* getOutputSectHdr(const std::string& pName);

  /// getOutputSectData - return a associated output section data
  SectionData* getOutputSectData(const std::string& pName);

  /// addMapping - add a mapping as creating one new output LDSection
  /// @param pName - a input section name
  /// @param pSection - the output LDSection*
  bool addMapping(const std::string& pName, LDSection* pSection);

  // -----  observers  ----- //
  bool empty() const
  { return m_LDSectionMap.empty(); }

  size_t size() const
  { return m_LDSectionMap.size(); }

  size_t capacity () const
  { return m_LDSectionMap.capacity(); }

  // -----  iterators  ----- //
  iterator find(const std::string& pName);

  iterator begin()
  { return m_LDSectionMap.begin(); }

  iterator end()
  { return m_LDSectionMap.end(); }

  const_iterator begin() const
  { return m_LDSectionMap.begin(); }

  const_iterator end() const
  { return m_LDSectionMap.end(); }

private:
  /// initOutputSectMap - initialize the map from input substr to associated
  /// output LDSection*
  void initOutputSectMap();

private:
  SectionMap& m_SectionNameMap;

  LDContext& m_Output;

  LDSectionMapTy m_LDSectionMap;
};

} // namespace of mcld

#endif

