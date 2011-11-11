//===- HashEntry.h ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_HASH_ENTRY_H
#define MCLD_HASH_ENTRY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include "llvm/ADT/StringRef.h"

namespace mcld {

/** \class HashEntry
 *  \brief HashEntry is the item in the bucket of hash table.
 *
 *  mcld::HashEntry illustrates the demand from mcld::HashTable.
 *  Since HashTable can change the definition of the HashEntry by changing
 *  the template argument. class mcld::HashEntry here is used to show the
 *  basic interfaces that HashTable requests. You can define your own entry
 *  of the hash table which has no relation to mcld::HashEntry
 *
 *  Since mcld::HashEntry here is a special class whose size is changing,
 *  derive a new class from it is risky. Make sure you understand what you
 *  are doing when you let a new class inherit from mcld::HashEntry.
 */
template <typename DataType>
class HashEntry : public DataType
{
public:
  typedef DataType value_type;
  typedef llvm::StringRef key_type;
  typedef DataType ValueTy;
  typedef llvm::StringRef KeyTy;

public:
  HashEntry();
  ~HashEntry(); // please do not inherit this class directly

  void setKey(llvm::StringRef pString);
  void setKey(const char* pKey, unsigned int pLength);
  llvm::StringRef getKey() const;
  bool equalKey(const char* pString) const;
  bool equalKey(const char* pString, size_t pLength);
  bool equalKey(const std::string& pString) const;
  bool equalKey(llvm::StringRef pString) const;

  llvm::StringRef toStringRef() const;
  const char* toCString() const;
  std::string toString() const;

  size_t sizeOfBytes() const
  { return sizeof(HashEntry<DataType>)+m_StrLength; }

public:
  unsigned int m_StrLength;
  char m_String[1];
};

/** \class HashBucket
 *  \brief HashBucket is an entry in the hash table.
 */
template<typename HashEntryTy>
class HashBucket
{
public:
  typedef HashEntryTy entry_type;

public:
  unsigned int FullHashValue;
  entry_type *Item;

public:
  static entry_type* getEmptyBucket();
  static entry_type* getTombstone();

};

// forward declarations
template<typename HashEntryTy, typename HashFunctionTy>
class BucketIteratorBase;

template<typename HashEntryTy, typename HashFunctionTy>
class HashIteratorBase;

/** \class HashTableImpl
 *  \brief HashTableImpl is the base class of HashTable.
 *
 *  HashTableImpl uses open-addressing, quadratic probing hash table.
 *  Quadratic probing hash table obviously has high performance when the
 *  load factor is less than 0.7.
 *  The drawback is that the number of the stored items can notbe more
 *  than the size of the hash table.
 *
 *  MCLinker tries to merge every things in the same HashEntry. It can
 *  keep every thing in the same cache line and improve the locality
 *  efficiently. HashTableImpl provides a template argument to change the
 *  definition of HashEntries.
 *
 *  HashEntryTy must provide getKey() and getKenLength() functions.
 *
 *  Different environments have different demands of HashFunctions. For
 *  example, on-device linkers needs a more light-weight hash function
 *  than static linkers. HashTableImpl also provides a template argument to
 *  change the hash functions.
 */
template<typename HashEntryTy,
         typename HashFunctionTy>
class HashTableImpl
{
friend class BucketIteratorBase<HashEntryTy, HashFunctionTy>;
friend class HashIteratorBase<HashEntryTy, HashFunctionTy>;

private:
  enum {
    NumOfInitBuckets = 17
  };

public:
  typedef HashBucket<HashEntryTy> BucketTy;

public:
  HashTableImpl();
  explicit HashTableImpl(unsigned int pInitSize);
  virtual ~HashTableImpl();

  // -----  observers  ----- //
  bool empty() const;

  size_t numOfBuckets() const
  { return m_NumOfBuckets; }

  size_t numOfItems() const
  { return m_NumOfItems; }

protected:
  void init(unsigned int pInitSize);

  /// rehash - check the load_factor, compute the new size, and then doRehash
  void rehash();

  /// doRehash - re-new the hash table, and rehash all elements into the new buckets
  void doRehash(unsigned int pNewSize);

  unsigned int lookUpBucketFor(llvm::StringRef pKey);

  /// findKey - finds an element with key pKey
  //  return the index of the element, or -1 when the element does not exist.
  int findKey(llvm::StringRef pKey) const;

  void remove(const HashEntryTy& pEntry);

protected:
  // Array of Buckets
  BucketTy *m_Buckets;
  unsigned int m_NumOfBuckets;
  unsigned int m_NumOfItems;
  unsigned int m_NumOfTombstones;
};

/** \class BucketIteratorBase
 *  \brief BucketIteratorBase follows the HashEntryTy with the same keys.
 */
template<typename HashEntryTy,
         typename HashFunctionTy>
class BucketIteratorBase
{
public:
  typedef HashBucket<HashEntryTy> BucketTy;
  typedef HashTableImpl<HashEntryTy, HashFunctionTy> HashTableImplTy;

public:
  BucketIteratorBase()
  : m_pHashTable(0), m_Key(), m_Probe(1), m_HashValue(0), m_Index(0)
  { }

  BucketIteratorBase(HashTableImplTy* pTable, llvm::StringRef pKey)
  : m_pHashTable(pTable), m_Key(pKey), m_Probe(1)
  {
    m_HashValue = HashFunctionTy(pKey);
    m_Index = m_HashValue & (pTable->m_NumOfBuckets-1);
  }

  BucketIteratorBase(const BucketIteratorBase& pCopy)
  : m_pHashTable(pCopy.m_pHashTable),
    m_Key(pCopy.m_Key),
    m_Probe(pCopy.m_Probe),
    m_Index(pCopy.m_Index),
    m_HashValue(pCopy.m_HashValue)
  { }

  inline BucketTy* getBucket() {
    if (0 == m_pHashTable)
      return 0;
    return m_pHashTable->m_Buckets[m_Index];
  }

  inline const BucketTy* getBucket() const {
    if (0 == m_pHashTable)
      return 0;
    return m_pHashTable->m_Buckets[m_Index];
  }

  inline HashEntryTy* getEntry() {
    if (0 == m_pHashTable)
      return 0;
    return m_pHashTable->m_Buckets[m_Index].Item;
  }

  inline const HashEntryTy* getEntry() const {
    if (0 == m_pHashTable)
      return 0;
    return m_pHashTable->m_Buckets[m_Index].Item;
  }

  inline void advance() {
    while(true) {
      BucketTy &bucket = m_pHashTable->m_Buckets[m_Index];

      // get end()
      if (BucketTy::getEmptyBucket() == bucket.Item) {
        m_pHashTable = 0;
        m_Index = 0;
        m_HashValue = 0;
        m_Probe = 1;
        return;
      }
      if (BucketTy::getTombstone() == bucket.Item) {
        // Ignore tombstones.
      }
      else if (m_HashValue == bucket.FullHashValue) {
        if (bucket.Item->equalKey(m_Key)) {
          return;
        }
      }
      m_Index = (m_Index + m_Probe) & (m_pHashTable->m_NumOfBuckets-1);
      ++m_Probe;
    }
  }

  bool operator==(const BucketIteratorBase& pCopy) const {
    return ((m_pHashTable == pCopy.m_pHashTable) &&
            (m_Key == pCopy.m_Key) &&
            (m_Index == pCopy.m_Index) &&
            (m_Probe == pCopy.m_Probe));
  }

  bool operator!=(const BucketIteratorBase& pCopy) const
  { return !(*this == pCopy); }

private:
  HashTableImplTy* m_pHashTable;
  llvm::StringRef m_Key;
  unsigned int m_Index;
  unsigned int m_Probe;
  unsigned int m_HashValue;
};
 
/** \class HashIteratorBase
 *  \brief HashIteratorBase walks over hash table by the natural layout of the
 *  buckets
 */
template<typename HashEntryTy,
         typename HashFunctionTy>
class HashIteratorBase
{
public:
  typedef HashBucket<HashEntryTy> BucketTy;
  typedef HashTableImpl<HashEntryTy, HashFunctionTy> HashTableImplTy;

public:
  HashIteratorBase()
  : m_pHashTable(0), m_Index(0)
  { }

  HashIteratorBase(HashTableImplTy* pTable,
                   unsigned int pIndex)
  : m_pHashTable(pTable), m_Index(pIndex)
  { }

  HashIteratorBase(const HashIteratorBase& pCopy)
  : m_pHashTable(pCopy.m_pHashTable), m_Index(pCopy.m_Index)
  { }

  inline BucketTy* getBucket() {
    if (0 == m_pHashTable)
      return 0;
    return m_pHashTable->m_Buckets[m_Index];
  }

  inline const BucketTy* getBucket() const {
    if (0 == m_pHashTable)
      return 0;
    return m_pHashTable->m_Buckets[m_Index];
  }

  inline HashEntryTy* getEntry() {
    if (0 == m_pHashTable)
      return 0;
    return m_pHashTable->m_Buckets[m_Index].Item;
  }

  inline const HashEntryTy* getEntry() const {
    if (0 == m_pHashTable)
      return 0;
    return m_pHashTable->m_Buckets[m_Index].Item;
  }

  inline void advance() {
    do {
      ++m_Index;
      if (m_pHashTable->m_NumOfBuckets == m_Index) { // to the end
        m_pHashTable = 0;
	m_Index = 0;
        return;
      }
    } while(BucketTy::getEmptyBucket() == m_pHashTable->m_Buckets[m_Index].Item || 
            BucketTy::getTombstone() == m_pHashTable->m_Buckets[m_Index].Item);
  }

  bool operator==(const HashIteratorBase& pCopy) const
  { return ((m_pHashTable == pCopy.m_pHashTable) &&
            (m_Index == pCopy.m_Index)); }

  bool operator!=(const HashIteratorBase& pCopy) const
  { return !(*this == pCopy); }

private:
  HashTableImplTy* m_pHashTable;
  unsigned int m_Index;

};

#include "HashEntry.tcc"

} // namespace of mcld

#endif

