/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_HASH_ITERATOR_H
#define MCLD_HASH_ITERATOR_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld {

/** \class ChainIteratorBase
 *  \brief ChaintIteratorBase follows the HashEntryTy with the same keys.
 */
template<typename HashTableImplTy>
class ChainIteratorBase
{
public:
  typedef HashTableImplTy hash_table;
  typedef typename HashTableImplTy::key_type key_type;
  typedef typename HashTableImplTy::entry_type entry_type;
  typedef typename HashTableImplTy::bucket_type bucket_type;

public:
  ChainIteratorBase()
  : m_pHashTable(0), m_Key(), m_Probe(1), m_HashValue(0), m_Index(0)
  { }

  ChainIteratorBase(HashTableImplTy* pTable, const key_type& pKey)
  : m_pHashTable(pTable), m_Key(pKey), m_Probe(1)
  {
    m_HashValue = pTable->hash(pKey);
    m_Index = m_HashValue % pTable->m_NumOfBuckets;
  }

  ChainIteratorBase(const ChainIteratorBase& pCopy)
  : m_pHashTable(pCopy.m_pHashTable),
    m_Key(pCopy.m_Key),
    m_Probe(pCopy.m_Probe),
    m_Index(pCopy.m_Index),
    m_HashValue(pCopy.m_HashValue)
  { }

  inline bucket_type* getBucket() {
    if (0 == m_pHashTable)
      return 0;
    return m_pHashTable->m_Buckets[m_Index];
  }

  inline const bucket_type* getBucket() const {
    if (0 == m_pHashTable)
      return 0;
    return m_pHashTable->m_Buckets[m_Index];
  }

  inline entry_type* getEntry() {
    if (0 == m_pHashTable)
      return 0;
    return m_pHashTable->m_Buckets[m_Index].Entry;
  }

  inline const entry_type* getEntry() const {
    if (0 == m_pHashTable)
      return 0;
    return m_pHashTable->m_Buckets[m_Index].Entry;
  }

  inline void reset() {
    m_pHashTable = 0;
    m_Index = 0;
    m_HashValue = 0;
    m_Probe = 1;
  }

  inline void advance() {
    static typename HashTableImplTy::key_compare comparator;
    while(true) {
      bucket_type &bucket = m_pHashTable->m_Buckets[m_Index];

      // get end()
      if (bucket_type::getEmptyBucket() == bucket.Entry) {
        reset();
        return;
      }
      if (bucket_type::getTombstone() == bucket.Entry) {
        // Ignore tombstones.
      }
      else if (m_HashValue == bucket.FullHashValue) {
        if (bucket.Entry->compare(m_Key))
          return;
      }
      m_Index = (m_Index + m_Probe) % (m_pHashTable->m_NumOfBuckets);
      ++m_Probe;
    }
  }

  bool operator==(const ChainIteratorBase& pCopy) const {
    return ((m_pHashTable == pCopy.m_pHashTable) &&
            (m_Key == pCopy.m_Key) &&
            (m_Index == pCopy.m_Index) &&
            (m_Probe == pCopy.m_Probe));
  }

  bool operator!=(const ChainIteratorBase& pCopy) const
  { return !(*this == pCopy); }

private:
  HashTableImplTy* m_pHashTable;
  key_type m_Key;
  unsigned int m_Index;
  unsigned int m_Probe;
  unsigned int m_HashValue;
};
 
/** \class EntryIteratorBase
 *  \brief EntryIteratorBase walks over hash table by the natural layout of the
 *  buckets
 */
template<typename HashTableImplTy>
class EntryIteratorBase
{
public:
  typedef HashTableImplTy hash_table;
  typedef typename HashTableImplTy::key_type key_type;
  typedef typename HashTableImplTy::entry_type entry_type;
  typedef typename HashTableImplTy::bucket_type bucket_type;

public:
  EntryIteratorBase()
  : m_pHashTable(0), m_Index(0)
  { }

  EntryIteratorBase(HashTableImplTy* pTable,
                   unsigned int pIndex)
  : m_pHashTable(pTable), m_Index(pIndex)
  { }

  EntryIteratorBase(const EntryIteratorBase& pCopy)
  : m_pHashTable(pCopy.m_pHashTable), m_Index(pCopy.m_Index)
  { }

  inline bucket_type* getBucket() {
    if (0 == m_pHashTable)
      return 0;
    return m_pHashTable->m_Buckets[m_Index];
  }

  inline const bucket_type* getBucket() const {
    if (0 == m_pHashTable)
      return 0;
    return m_pHashTable->m_Buckets[m_Index];
  }

  inline entry_type* getEntry() {
    if (0 == m_pHashTable)
      return 0;
    return m_pHashTable->m_Buckets[m_Index].Entry;
  }

  inline const entry_type* getEntry() const {
    if (0 == m_pHashTable)
      return 0;
    return m_pHashTable->m_Buckets[m_Index].Entry;
  }

  inline void reset() {
    m_pHashTable = 0;
    m_Index = 0;
  }

  inline void advance() {
    do {
      ++m_Index;
      if (m_pHashTable->m_NumOfBuckets == m_Index) { // to the end
        reset();
        return;
      }
    } while(bucket_type::getEmptyBucket() == m_pHashTable->m_Buckets[m_Index].Entry || 
            bucket_type::getTombstone() == m_pHashTable->m_Buckets[m_Index].Entry);
  }

  bool operator==(const EntryIteratorBase& pCopy) const
  { return ((m_pHashTable == pCopy.m_pHashTable) &&
            (m_Index == pCopy.m_Index)); }

  bool operator!=(const EntryIteratorBase& pCopy) const
  { return !(*this == pCopy); }

private:
  HashTableImplTy* m_pHashTable;
  unsigned int m_Index;

};

/** \class HashIterator
 *  \brief HashIterator provides a policy-based iterator.
 *
 *  HashTable has two kinds of iterators. One is used to traverse buckets
 *  with the same hash value; the other is used to traverse all entries of the
 *  hash table.
 *
 *  HashIterator is a template policy-based iterator, which can change its
 *  behavior by change the template argument IteratorBase. HashTable defines
 *  above two iterators by defining HashIterators with different IteratorBase.
 */
template<typename IteratorBase,
         typename Traits>
class HashIterator : public IteratorBase
{
public:
  typedef Traits                     traits;
  typedef typename traits::pointer   pointer;
  typedef typename traits::reference reference;
  typedef size_t                     size_type;
  typedef ptrdiff_t                  difference_type;
  typedef IteratorBase               Base;

  typedef HashIterator<IteratorBase,
                       Traits>             Self;

  typedef typename traits::nonconst_traits nonconst_traits;
  typedef HashIterator<IteratorBase,
                       nonconst_traits>    iterator;

  typedef typename traits::const_traits    const_traits;
  typedef HashIterator<IteratorBase,
                       const_traits>       const_iterator;
  typedef std::forward_iterator_tag        iterator_category;

public:
  HashIterator()
  : IteratorBase()
  { }

  HashIterator(typename IteratorBase::hash_table* pTable,
               const typename IteratorBase::key_type& pKey)
  : IteratorBase(pTable, pKey)
  { }

  HashIterator(typename IteratorBase::hash_table* pTable, unsigned int pIndex)
  : IteratorBase(pTable, pIndex)
  { }

  HashIterator(const HashIterator& pCopy)
  : IteratorBase(pCopy)
  { }

  ~HashIterator()
  { }

  // -----  operators  ----- //
  Self& operator++() {
    this->Base::advance();
    return *this;
  }

  Self operator++(int) {
    Self tmp = *this;
    this->Base::advance();
    return tmp;
  }
};

} // namespace of mcld

#endif

