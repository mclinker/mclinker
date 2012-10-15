//===- SectionRules.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Object/SectionRules.h>
#include <mcld/Object/SectionMap.h>
#include <mcld/LD/LDSection.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/ADT/StringHash.h>

#include <ctype.h>
#include <cstring>

using namespace mcld;

namespace {
  static StringHash<ES> hash_func;
} // end anonymous namespace

//===----------------------------------------------------------------------===//
// SectionRules
//===----------------------------------------------------------------------===//
SectionRules::SectionRules()
  : m_pNullSection(NULL) {
}

void SectionRules::append(const std::string& pName, LDSection& pSection)
{
  if (pName.empty()) {
    m_pNullSection = &pSection;
    return;
  }

  Rule rule;
  rule.hash = hash_func(pName);
  rule.substr = pName;
  rule.target = &pSection;
  m_RuleList.push_back(rule);
}

LDSection* SectionRules::getMatchedSection(const std::string& pName) const
{
  if (pName.empty())
    return m_pNullSection;

  uint32_t hash = hash_func(pName);
  RuleList::const_iterator rule, rEnd = m_RuleList.end();
  for (rule = m_RuleList.begin(); rule != rEnd; ++rule) {
    if (pName.size() < rule->substr.size())
      continue;
    if (!StringHash<ES>::may_include(rule->hash, hash))
      continue;

    if (0 == strncmp(pName.c_str(), rule->substr.c_str(), rule->substr.size()))
      return rule->target;
  }
  return NULL;
}

bool SectionRules::check() const
{
  // check if previous rules are the prefix of the current rule.
  RuleList::const_iterator rule, rEnd = m_RuleList.end();
  for (rule = m_RuleList.begin(); rule != rEnd; ++rule) {
    RuleList::const_iterator prev_rule = m_RuleList.begin();
    for (; prev_rule != rule; ++prev_rule) {
      if (prev_rule->substr.size() > rule->substr.size())
        continue;

      if (!StringHash<ES>::may_include(prev_rule->hash, rule->hash))
        continue;

      if (0 == strncmp(prev_rule->substr.c_str(),
                       rule->substr.c_str(),
                       prev_rule->substr.size())) {
        warning(diag::warn_rules_check_failed) << rule->substr
                                               << rule->target->name()
                                               << prev_rule->substr
                                               << prev_rule->target->name();
        return false;
      }
    }
  }
  return true;
}

