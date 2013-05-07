//===- ExprToken.h --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_EXPR_TOKEN_INTERFACE_H
#define MCLD_EXPR_TOKEN_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class ExprToken
 *  \brief This class defines the interfaces to an expression token.
 */

class ExprToken
{
public:
  enum Kind {
    Op,
    Opd,
    Func
  };

protected:
  ExprToken(Kind pKind)
    : m_Kind(pKind)
  {}

public:
  virtual ~ExprToken()
  {}

  virtual void dump() const = 0;

  Kind kind() const { return m_Kind; }

private:
  Kind m_Kind;
};

} // namespace of mcld

#endif

