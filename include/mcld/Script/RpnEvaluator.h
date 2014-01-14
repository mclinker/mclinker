//===- RpnEvaluator.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SCRIPT_RPNEVALUATOR_H
#define MCLD_SCRIPT_RPNEVALUATOR_H

namespace mcld {

class RpnExpr;
class Module;
class TargetLDBackend;

/** \class RpnEvaluator
 *  \brief RpnEvaluator evaluate a rpn expression
 */
class RpnEvaluator
{
public:
  RpnEvaluator(const Module& pModule, const TargetLDBackend& pBackend);

  // evaluate a valid expression and set the value in the second parameter
  bool eval(const RpnExpr& pExpr, uint64_t& pResult);

private:
  const Module& m_Module;
  const TargetLDBackend& m_Backend;
};

} // mcld

#endif
