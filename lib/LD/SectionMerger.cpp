//===- SectionMerger.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <cassert>
#include <cstring>
#include <mcld/Module.h>
#include <mcld/LinkerConfig.h>
#include <mcld/LD/SectionMerger.h>
#include <mcld/Object/SectionMap.h>

#include <iostream>
using namespace std;

using namespace mcld;

//===----------------------------------------------------------------------===//
// SectionMerger::Rule
//===----------------------------------------------------------------------===//
SectionMerger::Rule::Rule(const std::string& pSubStr, LDSection* pSection)
  : substr(pSubStr), target(pSection) {
}

//===----------------------------------------------------------------------===//
// SectionMerger
//===----------------------------------------------------------------------===//
SectionMerger::SectionMerger(const LinkerConfig& pConfig, Module& pModule)
  : m_SectionNameMap(pConfig.scripts().sectionMap()),
    m_Module(pModule) {
}

SectionMerger::~SectionMerger()
{
}

SectionMerger::iterator SectionMerger::find(const std::string& pName)
{
  iterator it;
  for (it = m_RuleList.begin(); it != m_RuleList.end(); ++it) {
    if (0 == strncmp(pName.c_str(),
                     (*it).substr.c_str(),
                     (*it).substr.length()))
      break;
    // wildcard to a user-defined output section.
    else if(0 == strcmp("*", (*it).substr.c_str()))
      break;
  }
  return it;
}

SectionMerger::const_iterator SectionMerger::find(const std::string& pName) const
{
  const_iterator it;
  for (it = m_RuleList.begin(); it != m_RuleList.end(); ++it) {
    if (0 == strncmp(pName.c_str(),
                     (*it).substr.c_str(),
                     (*it).substr.length()))
      break;
    // wildcard to a user-defined output section.
    else if(0 == strcmp("*", (*it).substr.c_str()))
      break;
  }
  return it;
}

LDSection* SectionMerger::getMatchedSection(const std::string& pName) const
{
  LDSection* section;
  const_iterator it = find(pName);

  // check if we can find a matched LDSection.
  // If not, we need to find it in output context. But this should be rare.
  if (it != m_RuleList.end())
    section = (*it).target;
  else
    section = m_Module.getSection(pName);

  assert(NULL != section);
  return section;
}

void SectionMerger::append(const std::string& pName, LDSection& pSection)
{
  iterator it = find(pName);
  if (it != m_RuleList.end()) {
    assert(NULL == (*it).target);
    (*it).target = &pSection;
  }
}

void SectionMerger::initOutputSectMap()
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
