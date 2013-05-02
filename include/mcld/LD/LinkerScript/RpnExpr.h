//===- RPNExpr.h ----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_RPN_EXPR_INTERFACE_H
#define MCLD_RPN_EXPR_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Support/Allocators.h>
#include <mcld/Config/Config.h>
#include <vector>

namespace mcld
{

class ExprToken;

/** \class RpnExpr
 *  \brief This class defines the interfaces to a rpn expression.
 */

class RpnExpr
{
public:
  typedef std::vector<ExprToken*> TokenQueue;
  typedef TokenQueue::const_iterator const_iterator;
  typedef TokenQueue::iterator iterator;

private:
  friend class Chunk<RpnExpr, MCLD_SYMBOLS_PER_INPUT>;
  RpnExpr();

public:
  ~RpnExpr();

  const_iterator begin() const { return m_TokenQueue.begin(); }
  iterator       begin()       { return m_TokenQueue.begin(); }
  const_iterator end()   const { return m_TokenQueue.end(); }
  iterator       end()         { return m_TokenQueue.end(); }

  size_t size() const { return m_TokenQueue.size(); }

  bool empty() const { return m_TokenQueue.empty(); }

  void dump() const;

  void append(ExprToken* pToken);

  /* factory methods */
  static RpnExpr* create();
  static void destroy(RpnExpr*& pRpnExpr);
  static void clear();

private:
  TokenQueue m_TokenQueue;
};

} // namespace of mcld

#endif

