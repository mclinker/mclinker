//===- SectionRules.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_OBJECT_SECTION_RULES_H
#define MCLD_OBJECT_SECTION_RULES_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <vector>
#include <string>
#include <llvm/Support/DataTypes.h>

namespace mcld {

class LDSection;
class SectionMap;

/** \class SectionRules
 *  \brief SectionRules maintains the section mapping rules
 *
 *  The section mapping rules come from four sources:
 *    - link script
 *    - target backend default setting,
 *    - format default setting, and
 *    - input object files.
 *
 *  SectionMap only covers the first two sources and SectionRules maintains
 *  all the rules from the four sources. We can say SectionMap collects the
 *  static rules - those rules that will not be changed at run-time. And
 *  SectionRules maintains both static rules and dynamic rules.
 */
class SectionRules
{
public:
  SectionRules();

  /// append - append a new mapping rule.
  /// appendRule does not check if the appended rule is duplicated.
  /// @param pName - the matched substring
  /// @parap pSection - the output section
  void append(const std::string& pName, LDSection& pSection);

  /// getMatchedSection - return the matched section by rules.
  /// @return if we can not find the matched section, return NULL.
  LDSection* getMatchedSection(const std::string& pName) const;

  /// check - check if all rules are legal.
  /// If one rule is imposible to matched, we say that rule is illegal.
  bool check() const;

  size_t size () const { return m_RuleList.size(); }
  bool   empty() const { return m_RuleList.empty(); }

private:
  struct Rule {
    uint32_t hash;
    std::string substr;
    LDSection* target;
  };

  typedef std::vector<Rule> RuleList;

private:
  RuleList m_RuleList;
  LDSection* m_pNullSection;
};

} // namespace of mcld

#endif

