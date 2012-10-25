//===- SectionRules.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/SectionRules.h>

#include <cassert>
#include <cstring>

#include <mcld/Module.h>
#include <mcld/LinkerConfig.h>
#include <mcld/Object/SectionMap.h>
#include <mcld/ADT/StringHash.h>

using namespace mcld;

namespace {
  static StringHash<ES> hash_func;
} // end anonymous namespace

//===----------------------------------------------------------------------===//
// SectionRules::Rule
//===----------------------------------------------------------------------===//
SectionRules::Rule::Rule(const std::string& pSubStr, LDSection* pSection)
  : substr(pSubStr), target(pSection) {
  hash = hash_func(pSubStr);
}

//===----------------------------------------------------------------------===//
// SectionRules
//===----------------------------------------------------------------------===//
SectionRules::SectionRules(const LinkerConfig& pConfig, Module& pModule)
  : m_SectionNameMap(pConfig.scripts().sectionMap()),
    m_Module(pModule) {
}

SectionRules::~SectionRules()
{
}

SectionRules::iterator SectionRules::find(const std::string& pName)
{
  uint32_t hash = hash_func(pName);
  RuleList::iterator rule, rEnd = m_RuleList.end();
  for (rule = m_RuleList.begin(); rule != rEnd; ++rule) {
    if (pName.size() < rule->substr.size())
      continue;
    if (!StringHash<ES>::may_include(rule->hash, hash))
      continue;

    if (0 == strncmp(pName.c_str(), rule->substr.c_str(), rule->substr.size()))
      return rule;
  }
  return rule;
}

SectionRules::const_iterator SectionRules::find(const std::string& pName) const
{
  uint32_t hash = hash_func(pName);
  RuleList::const_iterator rule, rEnd = m_RuleList.end();
  for (rule = m_RuleList.begin(); rule != rEnd; ++rule) {
    if (pName.size() < rule->substr.size())
      continue;
    if (!StringHash<ES>::may_include(rule->hash, hash))
      continue;

    if (0 == strncmp(pName.c_str(), rule->substr.c_str(), rule->substr.size()))
      return rule;
  }
  return rule;
}

LDSection* SectionRules::getMatchedSection(const std::string& pName) const
{
  LDSection* section;
  const_iterator it = find(pName);

  // check if we can find a matched LDSection.
  // If not, we need to find it in output context. But this should be rare.
  if (it != m_RuleList.end())
    section = (*it).target;
  else
    section = m_Module.getSection(pName);

  return section;
}

void SectionRules::append(const std::string& pName, LDSection& pSection)
{
  iterator it = find(pName);
  if (it != m_RuleList.end()) {
    assert(NULL == (*it).target);
    (*it).target = &pSection;
  }
}

void SectionRules::initOutputSectMap()
{
  // Based on SectionMap to initialize the map from a input substr to its 
  // associated output LDSection*
  SectionMap::const_iterator it;
  for (it = m_SectionNameMap.begin(); it != m_SectionNameMap.end(); ++it) {
    Rule rule(it->from, NULL);
    m_RuleList.push_back(rule);
  }
  assert(m_SectionNameMap.size() == m_RuleList.size());
}
