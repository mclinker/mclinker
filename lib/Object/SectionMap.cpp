//===- SectionMap.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Object/SectionMap.h>
#include <mcld/ADT/StringHash.h>
#include <cassert>
#include <cstring>

using namespace mcld;

namespace {
  static StringHash<ES> hash_func;
} // end anonymous namespace

SectionMap::NamePair SectionMap::NullName;

//===----------------------------------------------------------------------===//
// SectionMap::NamePair
//===----------------------------------------------------------------------===//
SectionMap::NamePair::NamePair()
  : hash(-1) {
}

SectionMap::NamePair::NamePair(const std::string& pFrom, const std::string& pTo)
  : from(pFrom), to(pTo) {
  hash = hash_func(pFrom);
}

bool SectionMap::NamePair::isNull() const
{
  return (&NullName == this);
}

//===----------------------------------------------------------------------===//
// SectionMap
//===----------------------------------------------------------------------===//
const SectionMap::NamePair& SectionMap::find(const std::string& pFrom) const
{
  unsigned int hash = hash_func(pFrom);
  NamePairList::const_iterator name_hash, nEnd = m_NamePairList.end();
  for (name_hash = m_NamePairList.begin(); name_hash != nEnd; ++name_hash) {
    if (matched(*name_hash, pFrom, hash)) {
      return *name_hash;
    }
  }
  return NullName;
}

SectionMap::NamePair& SectionMap::find(const std::string& pFrom)
{
  unsigned int hash = hash_func(pFrom);
  NamePairList::iterator name_hash, nEnd = m_NamePairList.end();
  for (name_hash = m_NamePairList.begin(); name_hash != nEnd; ++name_hash) {
    if (matched(*name_hash, pFrom, hash)) {
      return *name_hash;
    }
  }
  return NullName;
}

SectionMap::NamePair& SectionMap::append(const std::string &pFrom,
                                         const std::string &pTo,
                                         bool &pExist)
{
  NamePair& result = find(pFrom);
  if (!result.isNull()) {
    pExist = true;
    return result;
  }

  pExist = false;
  NamePair entry(pFrom, pTo);
  m_NamePairList.push_back(entry);
  return m_NamePairList.back();
}

bool SectionMap::matched(const NamePair& pNamePair,
                         const std::string& pInput,
                         unsigned int pHashValue) const
{
  if ('*' == pNamePair.from[0])
    return true;

  if (pNamePair.from.size() > pInput.size())
    return false;

  if (!StringHash<ES>::may_include(pNamePair.hash, pHashValue))
    return false;

  if (0 == strncmp(pInput.c_str(),
                   pNamePair.from.c_str(),
                   pNamePair.from.size())) {
    return true;
  }

  return false;
}

