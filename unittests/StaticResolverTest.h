//===- headerTest.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef STATICRESOLVER_TEST_H
#define STATICRESOLVER_TEST_H

#include <gtest.h>
#include <mcld/MC/MCLDInfo.h>

namespace mcld
{
class StaticResolver;
class ResolveInfoFactory;

} // namespace for mcld

namespace mcldtest
{

/** \class StaticResolverTest
 *  \brief The testcases for static resolver
 *
 *  \see StaticResolver 
 */
class StaticResolverTest : public ::testing::Test
{
public:
  // Constructor can do set-up work for all test here.
  StaticResolverTest();

  // Destructor can do clean-up work that doesn't throw exceptions here.
  virtual ~StaticResolverTest();

  // SetUp() will be called immediately before each test.
  virtual void SetUp();

  // TearDown() will be called immediately after each test.
  virtual void TearDown();

protected:
  mcld::StaticResolver* m_pResolver;
  mcld::ResolveInfoFactory* m_pFactory;
  mcld::MCLDInfo* m_pLDInfo;
  mcld::DiagnosticLineInfo* m_pLineInfo;
  mcld::DiagnosticPrinter* m_pPrinter;
};

} // namespace of mcldtest

#endif

