//===- HashTableTest.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "HashTableTest.h"
#include "mcld/ADT/HashEntry.h"
#include "mcld/ADT/HashTable.h"
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace mcld;
using namespace mcldtest;


// Constructor can do set-up work for all test here.
HashTableTest::HashTableTest()
{
}

// Destructor can do clean-up work that doesn't throw exceptions here.
HashTableTest::~HashTableTest()
{
}

// SetUp() will be called immediately before each test.
void HashTableTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void HashTableTest::TearDown()
{
}

//==========================================================================//
// Testcases
//
struct IntCompare
{
  bool operator()(int X, int Y) const
  { return (X==Y); }
};

struct IntHash
{
  size_t operator()(int pKey) const
  { return pKey; }
};

TEST_F( HashTableTest, constructor ) {
  typedef HashEntry<int, int, IntCompare> HashEntryType;
  HashTable<HashEntryType, IntHash, EntryFactory<HashEntryType> > hashTable(16);
  EXPECT_EQ(17, hashTable.numOfBuckets());
  EXPECT_TRUE(hashTable.empty());
  EXPECT_EQ(0, hashTable.numOfEntries());
}

TEST_F( HashTableTest, allocattion ) {
  typedef HashEntry<int, int, IntCompare> HashEntryType;
  typedef HashTable<HashEntryType, IntHash, EntryFactory<HashEntryType> > HashTableTy;
  HashTableTy *hashTable = new HashTableTy(22);

  bool exist;
  int key = 100;
  HashTableTy::entry_type* val = hashTable->insert(key, exist);
  val->setValue(999);
  EXPECT_FALSE(hashTable->empty());
  EXPECT_FALSE(exist);
  EXPECT_FALSE(NULL == val);
  HashTableTy::iterator entry = hashTable->find(key);
  EXPECT_EQ(999, entry.getEntry()->value());
  delete hashTable;
}

TEST_F( HashTableTest, alloc100 ) {
  typedef HashEntry<int, int, IntCompare> HashEntryType;
  typedef HashTable<HashEntryType, IntHash, EntryFactory<HashEntryType> > HashTableTy;
  HashTableTy *hashTable = new HashTableTy(22);

  bool exist;
  HashTableTy::entry_type* entry = 0;
  for (unsigned int key=0; key<100; ++key) {
    entry = hashTable->insert(key, exist);
    EXPECT_FALSE(hashTable->empty());
    EXPECT_FALSE(exist);
    EXPECT_FALSE(NULL == entry);
    EXPECT_EQ(key, entry->key());
    entry->setValue(key+10);
  }

  EXPECT_FALSE(hashTable->empty());
  EXPECT_EQ(100, hashTable->numOfEntries());
  EXPECT_EQ(197, hashTable->numOfBuckets());
}

