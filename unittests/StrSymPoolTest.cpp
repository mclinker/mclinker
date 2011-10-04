/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#include <mcld/LD/StrSymPool.h>
#include "StrSymPoolTest.h"
#include <string>
#include <cstdio>

using namespace std;
using namespace mcld;
using namespace mcldtest;


// Constructor can do set-up work for all test here.
StrSymPoolTest::StrSymPoolTest()
{
	// create testee. modify it if need
  Resolver resolver;
	m_pTestee = new StrSymPool(1, 1, resolver);
}

// Destructor can do clean-up work that doesn't throw exceptions here.
StrSymPoolTest::~StrSymPoolTest()
{
	delete m_pTestee;
}

// SetUp() will be called immediately before each test.
void StrSymPoolTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void StrSymPoolTest::TearDown()
{
}

//==========================================================================//
// Testcases
//


TEST_F( StrSymPoolTest, insertString ) {
  const char *s1 = "Hello MCLinker";
  const char *result1 = m_pTestee->insertString(s1);
  ASSERT_NE(s1, result1);
  ASSERT_STREQ(s1, result1);
}

TEST_F( StrSymPoolTest, insertSameString ) {
  const char *s1 = "Hello MCLinker";
  string s2(s1);
  const char *result1 = m_pTestee->insertString(s1);
  const char *result2 = m_pTestee->insertString(s2.c_str());
  ASSERT_STREQ(s1, result1);
  ASSERT_STREQ(s2.c_str(), result2);
  ASSERT_EQ(result1, result2);
}

TEST_F( StrSymPoolTest, insert_local_defined_Symbol ) {
  const char *name = "Hello MCLinker";
  bool isDyn = false;
  LDSymbol::Type type = LDSymbol::Defined;
  LDSymbol::Binding binding = LDSymbol::Local;
  const llvm::MCSectionData *section = 0;
  uint64_t value = 0;
  uint64_t size = 0;
  uint8_t other = 0;

  LDSymbol *sym =  m_pTestee->insertSymbol(name,
                                           isDyn,
                                           type,
                                           binding,
                                           section,
                                           value,
                                           size,
                                           other);
  ASSERT_NE(name, sym->name());
  ASSERT_STREQ(name, sym->name());
  ASSERT_EQ(isDyn, sym->isDyn());
  ASSERT_EQ(type, sym->type());
  ASSERT_EQ(binding, sym->binding());
  ASSERT_EQ(section, sym->section());
  ASSERT_EQ(value, sym->value());
  ASSERT_EQ(size, sym->size());
  ASSERT_EQ(other, sym->other());

  LDSymbol *sym2 =  m_pTestee->insertSymbol(name,
                                            isDyn,
                                            type,
                                            binding,
                                            section,
                                            value,
                                            size,
                                            other);
  ASSERT_NE(name, sym2->name());
  ASSERT_STREQ(name, sym2->name());
  ASSERT_EQ(isDyn, sym2->isDyn());
  ASSERT_EQ(type, sym2->type());
  ASSERT_EQ(binding, sym2->binding());
  ASSERT_EQ(section, sym2->section());
  ASSERT_EQ(value, sym2->value());
  ASSERT_EQ(size, sym2->size());
  ASSERT_EQ(other, sym2->other());


  ASSERT_NE(sym, sym2);
}

TEST_F( StrSymPoolTest, insert_global_reference_Symbol ) {
  const char *name = "Hello MCLinker";
  bool isDyn = false;
  LDSymbol::Type type = LDSymbol::Reference;
  LDSymbol::Binding binding = LDSymbol::Global;
  const llvm::MCSectionData *section = 0;
  uint64_t value = 0;
  uint64_t size = 0;
  uint8_t other = 0;

  LDSymbol *sym =  m_pTestee->insertSymbol(name,
                                           isDyn,
                                           type,
                                           binding,
                                           section,
                                           value,
                                           size,
                                           other);
  ASSERT_NE(name, sym->name());
  ASSERT_STREQ(name, sym->name());
  ASSERT_EQ(isDyn, sym->isDyn());
  ASSERT_EQ(type, sym->type());
  ASSERT_EQ(binding, sym->binding());
  ASSERT_EQ(section, sym->section());
  ASSERT_EQ(value, sym->value());
  ASSERT_EQ(size, sym->size());
  ASSERT_EQ(other, sym->other());


  LDSymbol *sym2 =  m_pTestee->insertSymbol(name,
                                            isDyn,
                                            type,
                                            binding,
                                            section,
                                            value,
                                            size,
                                            other);


  ASSERT_EQ(sym, sym2);


  LDSymbol *sym3 =  m_pTestee->insertSymbol("Different symbol",
                                            isDyn,
                                            type,
                                            binding,
                                            section,
                                            value,
                                            size,
                                            other);

  ASSERT_NE(sym, sym3);
}


TEST_F( StrSymPoolTest, insertSymbol_after_insert_same_string ) {
  const char *name = "Hello MCLinker";
  bool isDyn = false;
  LDSymbol::Type type = LDSymbol::Defined;
  LDSymbol::Binding binding = LDSymbol::Global;
  const llvm::MCSectionData *section = 0;
  uint64_t value = 0;
  uint64_t size = 0;
  uint8_t other = 0;

  const char *result1 =  m_pTestee->insertString(name);
  LDSymbol *sym =  m_pTestee->insertSymbol(name,
                                           isDyn,
                                           type,
                                           binding,
                                           section,
                                           value,
                                           size,
                                           other);

  ASSERT_STREQ(name, sym->name());
  ASSERT_EQ(result1, sym->name());

  char s[16];
  strcpy(s, result1);
  const char *result2 = m_pTestee->insertString(result1);
  const char *result3 = m_pTestee->insertString(s);

  ASSERT_EQ(result1, result2);
  ASSERT_EQ(result1, result3);
}


TEST_F( StrSymPoolTest, insert_16384_weak_reference_symbols ) {
  char name[16];
  bool isDyn = false;
  LDSymbol::Type type = LDSymbol::Reference;
  LDSymbol::Binding binding = LDSymbol::Weak;
  const llvm::MCSectionData *section = 0;
  uint64_t value = 0;
  uint64_t size = 0;
  uint8_t other = 0;
  strcpy(name, "Hello MCLinker");
  LDSymbol *syms[128][128];
  for(int i=0; i<128 ;++i) {
    name[0] = i;
    for(int j=0; j<128 ;++j) {
      name[1] = j;
      syms[i][j] =  m_pTestee->insertSymbol(name,
                                            isDyn,
                                            type,
                                            binding,
                                            section,
                                            value,
                                            size,
                                            other);

      ASSERT_STREQ(name, syms[i][j]->name());
    }
  }
  for(int i=127; i>=0 ;--i) {
    name[0] = i;
    for(int j=0; j<128 ;++j) {
      name[1] = j;
      LDSymbol *sym =  m_pTestee->insertSymbol(name,
                                               isDyn,
                                               type,
                                               binding,
                                               section,
                                               value,
                                               size,
                                               other);
      ASSERT_EQ(sym, syms[i][j]);
    }
  }
  for(int i=0; i<128 ;++i) {
    name[0] = i;
    for(int j=0; j<128 ;++j) {
      name[1] = j;
      LDSymbol *sym =  m_pTestee->insertSymbol(name,
                                               isDyn,
                                               type,
                                               binding,
                                               section,
                                               value,
                                               size,
                                               other);
      ASSERT_EQ(sym, syms[i][j]);
    }
  }
}
