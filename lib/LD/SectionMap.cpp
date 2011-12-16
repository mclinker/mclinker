//===- SectionMap.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <cassert>
#include <cstring>
#include <mcld/LD/SectionMap.h>

using namespace mcld;

//==========================
// SectionMap

SectionMap::SectionMap()
{}

SectionMap::~SectionMap()
{}

bool SectionMap::addNameMapping(const std::string& pFrom, const std::string& pTo)
{
  iterator it;
  // check if the mapping exists in the map already
  for (it = begin(); it != end(); ++it) {
    if (pFrom == (*it).first)
      return false;
  }
  m_NameMap.push_back(NamePairTy(pFrom, pTo));
  return true;
}

const std::string& SectionMap::getOutputSectionName(const std::string& pFrom)
{
  iterator it;
  for (it = begin(); it != end(); ++it) {
    if (0 == strncmp((*it).first.c_str(), pFrom.c_str(), (*it).first.length()))
      break;
  }
  assert(end() != it);
  return (*it).second;
}
