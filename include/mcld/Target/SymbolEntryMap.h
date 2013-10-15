//===- SymbolEntryMap.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TARGET_SYMBOL_ENTRY_MAP_H
#define MCLD_TARGET_SYMBOL_ENTRY_MAP_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <vector>
#include <list>

namespace mcld {

class ResolveInfo;

/** \class SymbolEntryMap
 *  \brief SymbolEntryMap is a <const ResolveInfo*, ENTRY*> map.
 */
template<typename ENTRY>
class SymbolEntryMap
{
public:
  typedef ENTRY EntryType;

private:
  struct EntryPair {
    EntryPair(EntryType* pEntry1, EntryType* pEntry2)
     : entry1(pEntry1), entry2(pEntry2)
    {}

    EntryType* entry1;
    EntryType* entry2;
  };

  /// EntryOrPair - A symbol may mapping to a signal entry or a pair of entries,
  /// user is responsible for the type of Mapping.entry
  union EntryOrPair {
    EntryType* entry_ptr;
    EntryPair* pair_ptr;
  };

  struct Mapping {
    const ResolveInfo* symbol;
    EntryOrPair entry;
  };

  typedef std::vector<Mapping> SymbolEntryPool;
  typedef std::list<EntryPair> PairListType;

public:
  typedef typename SymbolEntryPool::iterator iterator;
  typedef typename SymbolEntryPool::const_iterator const_iterator;

public:
  /// lookUp - look up the entry mapping to pSymbol
  const EntryType* lookUp(const ResolveInfo& pSymbol) const;
  EntryType*       lookUp(const ResolveInfo& pSymbol);

  /// lookUpFirstEntry - look up the first entry mapping to pSymbol
  const EntryType* lookUpFirstEntry(const ResolveInfo& pSymbol) const;
  EntryType*       lookUpFirstEntry(const ResolveInfo& pSymbol);

  /// lookUpSecondEntry - look up the second entry mapping to pSymbol
  const EntryType* lookUpSecondEntry(const ResolveInfo& pSymbol) const;
  EntryType*       lookUpSecondEntry(const ResolveInfo& pSymbol);

  void record(const ResolveInfo& pSymbol, EntryType& pEntry);
  void record(const ResolveInfo& pSymbol,
              EntryType& pEntry1,
              EntryType& pEntry2);

  bool   empty() const { return m_Pool.empty(); }
  size_t size () const { return m_Pool.size(); }

  const_iterator begin() const { return m_Pool.begin(); }
  iterator       begin()       { return m_Pool.begin(); }
  const_iterator end  () const { return m_Pool.end();   }
  iterator       end  ()       { return m_Pool.end();   }

  void reserve(size_t pSize) { m_Pool.reserve(pSize); }

private:
  SymbolEntryPool m_Pool;

  /// m_Pairs - the EntryPairs
  PairListType m_Pairs;
};

template<typename EntryType>
const EntryType*
SymbolEntryMap<EntryType>::lookUp(const ResolveInfo& pSymbol) const
{
  const_iterator mapping, mEnd = m_Pool.end();
  for (mapping = m_Pool.begin(); mapping != mEnd; ++mapping) {
    if (mapping->symbol == &pSymbol) {
      return mapping->entry.entry_ptr;
    }
  }

  return NULL;
}

template<typename EntryType>
EntryType*
SymbolEntryMap<EntryType>::lookUp(const ResolveInfo& pSymbol)
{
  iterator mapping, mEnd = m_Pool.end();
  for (mapping = m_Pool.begin(); mapping != mEnd; ++mapping) {
    if (mapping->symbol == &pSymbol) {
      return mapping->entry.entry_ptr;
    }
  }

  return NULL;
}

template<typename EntryType>
const EntryType*
SymbolEntryMap<EntryType>::lookUpFirstEntry(const ResolveInfo& pSymbol) const
{
  const_iterator mapping, mEnd = m_Pool.end();
  for (mapping = m_Pool.begin(); mapping != mEnd; ++mapping) {
    if (mapping->symbol == &pSymbol) {
      return mapping->entry.pair_ptr->entry1;
    }
  }

  return NULL;
}

template<typename EntryType>
EntryType*
SymbolEntryMap<EntryType>::lookUpFirstEntry(const ResolveInfo& pSymbol)
{
  const_iterator mapping, mEnd = m_Pool.end();
  for (mapping = m_Pool.begin(); mapping != mEnd; ++mapping) {
    if (mapping->symbol == &pSymbol) {
      return mapping->entry.pair_ptr->entry1;
    }
  }

  return NULL;
}

template<typename EntryType>
const EntryType*
SymbolEntryMap<EntryType>::lookUpSecondEntry(const ResolveInfo& pSymbol) const
{
  const_iterator mapping, mEnd = m_Pool.end();
  for (mapping = m_Pool.begin(); mapping != mEnd; ++mapping) {
    if (mapping->symbol == &pSymbol) {
      return mapping->entry.pair_ptr->entry2;
    }
  }

  return NULL;
}

template<typename EntryType>
EntryType*
SymbolEntryMap<EntryType>::lookUpSecondEntry(const ResolveInfo& pSymbol)
{
  const_iterator mapping, mEnd = m_Pool.end();
  for (mapping = m_Pool.begin(); mapping != mEnd; ++mapping) {
    if (mapping->symbol == &pSymbol) {
      return mapping->entry.pair_ptr->entry2;
    }
  }

  return NULL;
}

template<typename EntryType>
void
SymbolEntryMap<EntryType>::record(const ResolveInfo& pSymbol, EntryType& pEntry)
{
  Mapping mapping;
  mapping.symbol = &pSymbol;
  mapping.entry.entry_ptr = &pEntry;
  m_Pool.push_back(mapping);
}

template<typename EntryType>
void
SymbolEntryMap<EntryType>::record(const ResolveInfo& pSymbol,
                                  EntryType& pEntry1,
                                  EntryType& pEntry2)
{
  Mapping mapping;
  mapping.symbol = &pSymbol;
  m_Pairs.push_back(EntryPair(&pEntry1, &pEntry2));
  mapping.entry.pair_ptr = &m_Pairs.back();
  m_Pool.push_back(mapping);
}

} // namespace of mcld

#endif

