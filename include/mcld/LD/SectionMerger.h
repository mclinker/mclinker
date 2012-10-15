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

namespace mcld {

class Module;
class LinkerConfig;
class SectionMap;
class FragmentLinker;

/** \class SectionMerger
 *  \brief maintain the mappings of substr of input section name to associated
 *         output section (data)
 */
class SectionMerger
{
public:
  SectionMerger(const LinkerConfig& pConfig, Module& pModule);
  ~SectionMerger();

  /// getMatchedSection - return the matched section by rules.
  /// @return if we can not find the matched section, return NULL.
  LDSection* getMatchedSection(const std::string& pName) const;

  /// append - append a new mapping rule.
  /// appendRule does not check if the appended rule is duplicated.
  /// @param pName - the matched substring
  /// @parap pSection - the output section
  void append(const std::string& pName, LDSection& pSection);

  // -----  observers  ----- //
  bool empty() const
  { return m_LDSectionMap.empty(); }

  size_t size() const
  { return m_LDSectionMap.size(); }

  /// initOutputSectMap - initialize the map from input substr to associated
  /// output LDSection*
  void initOutputSectMap();

private:
  struct NameSectPair {
    std::string inputSubStr;
    LDSection* outputSection;
  };

  typedef std::vector<NameSectPair> LDSectionMapTy;

  typedef LDSectionMapTy::iterator iterator;
  typedef LDSectionMapTy::const_iterator const_iterator;

private:
  // -----  iterators  ----- //
  const_iterator find(const std::string& pName) const;
  iterator       find(const std::string& pName);

  const_iterator begin() const { return m_LDSectionMap.begin(); }
  iterator       begin()       { return m_LDSectionMap.begin(); }
  const_iterator end  () const { return m_LDSectionMap.end(); }
  iterator       end  ()       { return m_LDSectionMap.end(); }

private:
  const SectionMap& m_SectionNameMap;
  Module& m_Module;
  LDSectionMapTy m_LDSectionMap;
};

} // namespace of mcld

#endif

