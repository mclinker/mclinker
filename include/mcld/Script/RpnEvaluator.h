//===- RpnEvaluator.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_RPN_EVALUATOR_H
#define MCLD_RPN_EVALUATOR_H

namespace mcld {

class RpnExpr;
class Module;

/** \class RpnEvaluator
 *  \brief RpnEvaluator evaluate a rpn expression
 */
class RpnEvaluator
{
public:
  RpnEvaluator(const Module& pModule);

  // evaluate a valid expression and set the value in the second parameter
  bool eval(const RpnExpr& pExpr, uint64_t& pResult);

private:
  const Module& m_Module;
};

} // mcld

#endif
