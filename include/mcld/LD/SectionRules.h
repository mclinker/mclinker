//===- SectionRules.h -----------------------------------------------------===//
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

/** \class SectionRules
 *  \brief maintain the mappings of substr of input section name to associated
 *         output section (data)
 */
class SectionRules
{
public:
  SectionRules(const LinkerConfig& pConfig, Module& pModule);
  ~SectionRules();

  /// getMatchedSection - return the matched section by rules.
  /// @return if we can not find the matched section, return NULL.
  LDSection* getMatchedSection(const std::string& pName) const;

  /// append - append a new mapping rule.
  /// appendRule does not check if the appended rule is duplicated.
  /// @param pName - the matched substring
  /// @parap pSection - the output section
  void append(const std::string& pName, LDSection& pSection);

  size_t size () const { return m_RuleList.size(); }
  bool   empty() const { return m_RuleList.empty(); }

  /// initOutputSectMap - initialize the map from input substr to associated
  /// output LDSection*
  void initOutputSectMap();

private:
  struct Rule {
  public:
    Rule(const std::string& pSubStr, LDSection* pTarget);

  public:
    uint32_t hash;
    std::string substr;
    LDSection* target;
  };

  typedef std::vector<Rule> RuleList;

  typedef RuleList::iterator iterator;
  typedef RuleList::const_iterator const_iterator;

private:
  // -----  iterators  ----- //
  const_iterator find(const std::string& pName) const;
  iterator       find(const std::string& pName);

private:
  const SectionMap& m_SectionNameMap;
  Module& m_Module;
  RuleList m_RuleList;
};

} // namespace of mcld

#endif

