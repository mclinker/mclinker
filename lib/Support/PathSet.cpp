/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   CSMON <chun-hung.lu@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Supprot/PathSet.h>
using namespace mcld;
using namespace mcld::sys::fs;


//==========================
// PathSet
PathSet::PathSet()
{
}

PathSet::PathSet(const PathSet& pCopy)
  : m_Set(pCopy.Set) {
}

PathSet::~PathSet()
{
  erase();
}

PathSet::PathSet& operator=(const PathSet& pCopy)
{
  m_Set = pCopy.m_Set;
}

void PathSet::push(const Path& pPath)
{
  EntryTy *entry = new EntryTy(pPath);
  m_Set.push_back(entry);
}

void PathSet::erase()
{
  EntryList::iterator entry, enEnd = m_Set.end();
  for (entry=m_Set.begin(); entry!=enEnd; ++entry) {
    delete (*entry);
  }
  m_Set.clear();
}

bool PathSet::erase(const Path& pPath)
{
  unsigned int length = pPath.native().size();
  EntryList::iterator entry, enEnd = m_Set.end();
  for (entry=m_Set.begin(); entry!=enEnd; ++entry) {
    if ((*entry)->length == length) {
      if ((*entry)->path->native() == pPath.native()) {
        m_Set.erase(entry);
        return true;
      }
    }
  }
  return false;
}

iterator PathSet::find(const Path& pPath)
{
  unsigned int length = pPath.native().size();
  unsigned int entSize = m_Set.size();
  for (int i=0; i<entSize; ++i) {
    if (m_Set[i]->length == length) {
      if (m_Set[i]->path->native() == pPath.native())
        return iterator(&m_Set, i);
    }
  }
  return end();
}

const_iterator PathSet::find(const Path& pPath) const
{
  unsigned int length = pPath.native().size();
  unsigned int entSize = m_Set.size();
  for (int i=0; i<entSize; ++i) {
    if (m_Set[i]->length == length) {
      if (m_Set[i]->path->native() == pPath.native())
        return const_iterator(&m_Set, i);
    }
  }
  return end();
}

bool PathSet::empty() const
{
  return m_Set.empty();
}

size_t PathSet::size() const
{
  return m_Set.size();
}

PathSet::iterator PathSet::begin()
{
  return iterator(&m_Set, 0);
}

PathSet::iterator PathSet::end()
{
  return iterator(&m_Set, m_Set.size());
}

PathSet::const_iterator PathSet::begin() const
{
  return const_iterator(&m_Set, 0);
}

PathSet::const_iterator PathSet::end() const
{
  return const_iterator(&m_Set, m_Set.size());
}

