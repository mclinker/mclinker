//===- SectionMap.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Object/SectionMap.h>
#include <mcld/Script/WildcardPattern.h>
#include <mcld/Script/StringList.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/SectionData.h>
#include <mcld/Fragment/NullFragment.h>
#include <cassert>
#include <cstring>
#include <climits>
#include <fnmatch.h>

using namespace mcld;
//===----------------------------------------------------------------------===//
// SectionMap::Input
//===----------------------------------------------------------------------===//
SectionMap::Input::Input(const std::string& pName)
  : m_Policy(InputSectDesc::NoKeep)
{
  m_Spec.m_pWildcardFile =
    WildcardPattern::create("*", WildcardPattern::SORT_NONE);
  m_Spec.m_pExcludeFiles = NULL;

  StringList* sections = StringList::create();
  sections->push_back(
    WildcardPattern::create(pName, WildcardPattern::SORT_NONE));
  m_Spec.m_pWildcardSections = sections;

  m_pSection = LDSection::Create(pName, LDFileFormat::Regular, 0, 0);
  SectionData* sd = SectionData::Create(*m_pSection);
  m_pSection->setSectionData(sd);
}

//===----------------------------------------------------------------------===//
// SectionMap::Output
//===----------------------------------------------------------------------===//
SectionMap::Output::Output(const std::string& pName)
  : m_Name(pName),
    m_Order(UINT_MAX)
{
  m_Prolog.m_pVMA = NULL;
  m_Prolog.m_Type = OutputSectDesc::LOAD;
  m_Prolog.m_pLMA = NULL;
  m_Prolog.m_pAlign = NULL;
  m_Prolog.m_pSubAlign = NULL;
  m_Prolog.m_Constraint = OutputSectDesc::NO_CONSTRAINT;

  m_Epilog.m_pRegion = NULL;
  m_Epilog.m_pLMARegion = NULL;
  m_Epilog.m_pPhdrs = NULL;
  m_Epilog.m_pFillExp = NULL;

  m_pSection = LDSection::Create(pName, LDFileFormat::Regular, 0, 0);
  SectionData* sd = SectionData::Create(*m_pSection);
  m_pSection->setSectionData(sd);
}

bool SectionMap::Output::hasContent() const
{
  return m_pSection != NULL && m_pSection->size() != 0;
}

//===----------------------------------------------------------------------===//
// SectionMap
//===----------------------------------------------------------------------===//
SectionMap::~SectionMap()
{
  iterator out, outBegin = begin(), outEnd = end();
  for (out = outBegin; out != outEnd; ++out) {
    if (*out != NULL) {
      Output::iterator in, inBegin = (*out)->begin(), inEnd = (*out)->end();
      for (in = inBegin; in != inEnd; ++in) {
        if (*in != NULL)
          delete *in;
      }
      delete *out;
    }
  }
}

SectionMap::const_mapping
SectionMap::find(const std::string& pInputSection) const
{
  const_iterator out, outBegin = begin(), outEnd = end();
  for (out = outBegin; out != outEnd; ++out) {
    Output::const_iterator in, inBegin = (*out)->begin(), inEnd = (*out)->end();
    for (in = inBegin; in != inEnd; ++in) {
      if (matched(**in, pInputSection))
        return std::make_pair(*out, *in);
    }
  }
  return std::make_pair((const Output*)NULL, (const Input*)NULL);
}

SectionMap::mapping SectionMap::find(const std::string& pInputSection)
{
  iterator out, outBegin = begin(), outEnd = end();
  for (out = outBegin; out != outEnd; ++out) {
    Output::iterator in, inBegin = (*out)->begin(), inEnd = (*out)->end();
    for (in = inBegin; in != inEnd; ++in) {
      if (matched(**in, pInputSection))
        return std::make_pair(*out, *in);
    }
  }
  return std::make_pair((Output*)NULL, (Input*)NULL);
}

std::pair<SectionMap::mapping, bool>
SectionMap::insert(const std::string& pInputSection,
                   const std::string& pOutputSection)
{
  iterator out, outBegin = begin(), outEnd = end();
  for (out = outBegin; out != outEnd; ++out) {
    if ((*out)->name().compare(pOutputSection) == 0)
      break;
  }
  if (out != end()) {
    Output::iterator in, inBegin = (*out)->begin(), inEnd = (*out)->end();
    for (in = inBegin; in != inEnd; ++in) {
      if ((*in)->getSection()->name().compare(pInputSection) == 0)
        break;
    }

    if (in != (*out)->end()) {
      return std::make_pair(std::make_pair(*out, *in), false);
    } else {
      Input* input = new Input(pInputSection);
      (*out)->append(input);
      return std::make_pair(std::make_pair(*out, input), true);
    }
  }

  Output* output = new Output(pOutputSection);
  m_OutputDescList.push_back(output);
  Input* input = new Input(pInputSection);
  output->append(input);

  return std::make_pair(std::make_pair(output, input), true);
}

bool SectionMap::matched(const Input& pInput, const std::string& pString) const
{
  if (pInput.spec().hasSections()) {
    for (StringList::const_iterator it = pInput.spec().sections().begin(),
      ie = pInput.spec().sections().end(); it != ie; ++it) {
      if (fnmatch((*it)->name().c_str(), pString.c_str(), 0) == 0)
        return true;
    }
  }
  return false;
}
