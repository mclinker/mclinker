//===- HashTableTest.cpp ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "HashTableTest.h"
#include "mcld/ADT/HashTable.h"
// #include <mcld/ADT/HashEntry.h> // DO NOT INCLUDE, it is a part of test

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
class Int
{
public:
  int value;
};

TEST_F( HashTableTest, constructor ) {
  HashTable<HashEntry<Int>, StringHash<ELF> > hashTable(16);
  EXPECT_EQ(17, hashTable.numOfBuckets());
  EXPECT_TRUE(hashTable.empty());
  EXPECT_EQ(0, hashTable.numOfItems());
}

TEST_F( HashTableTest, allocattion ) {
  typedef HashTable<HashEntry<Int>, StringHash<RS> > HashTableTy;
  HashTableTy *hashTable = new HashTableTy(22);
  char *str = (char*)malloc(10);
  sprintf( str, "%d\0", 12345678);
  cerr << str << endl;
  llvm::StringRef key(str, 9);
  bool exist = false;
  HashTableTy::value_type* val = hashTable->emplace(key, exist);
  EXPECT_FALSE(hashTable->empty());
  EXPECT_FALSE(exist);
  EXPECT_FALSE(NULL == val);
  val->value = 999;
  HashTableTy::iterator entry = hashTable->find( key );
  cerr << entry.getEntry()->value << endl;
  delete hashTable;
}
