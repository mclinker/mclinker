//===- RPNExpr.cpp --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/RpnExpr.h>
#include <mcld/Script/ExprToken.h>
#include <mcld/Support/GCFactory.h>
#include <llvm/Support/ManagedStatic.h>

using namespace mcld;

typedef GCFactory<RpnExpr, MCLD_SYMBOLS_PER_INPUT> ExprFactory;
static llvm::ManagedStatic<ExprFactory> g_ExprFactory;

//===----------------------------------------------------------------------===//
// RpnExpr
//===----------------------------------------------------------------------===//
RpnExpr::RpnExpr()
{
}

RpnExpr::~RpnExpr()
{
}

void RpnExpr::dump() const
{
  for (const_iterator it = begin(), ie = end(); it != ie; ++it)
    (*it)->dump();
}

void RpnExpr::append(ExprToken* pToken)
{
  m_TokenQueue.push_back(pToken);
}

RpnExpr* RpnExpr::create()
{
  RpnExpr* result = g_ExprFactory->allocate();
  new (result) RpnExpr();
  return result;
}

void RpnExpr::destroy(RpnExpr*& pRpnExpr)
{
  g_ExprFactory->destroy(pRpnExpr);
  g_ExprFactory->deallocate(pRpnExpr);
  pRpnExpr = NULL;
}

void RpnExpr::clear()
{
  g_ExprFactory->clear();
}

