//===- RpnEvaluator.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/MsgHandling.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/Script/RpnExpr.h>
#include <mcld/Script/RpnEvaluator.h>
#include <mcld/Script/ExprToken.h>
#include <mcld/Script/Operator.h>
#include <mcld/Script/Operand.h>
#include <mcld/Module.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/DataTypes.h>
#include <stack>
#include <cassert>

using namespace mcld;

RpnEvaluator::RpnEvaluator(const Module& pModule)
  : m_Module(pModule)
{
}

bool RpnEvaluator::eval(const RpnExpr& pExpr, uint64_t& pResult)
{
  std::stack<uint64_t> operandStack;
  for (RpnExpr::const_iterator it = pExpr.begin(), ie = pExpr.end(); it != ie;
    ++it) {
    switch((*it)->kind()) {
    case ExprToken::OPERATOR: {
      Operator* op = llvm::cast<Operator>(*it);
      switch (op->arity()) {
      case Operator::Unary: {
        uint64_t opd = operandStack.top();
        operandStack.pop();
        op->appendOperand(opd);
        operandStack.push(op->eval());
        break;
      }
      case Operator::Binary: {
        uint64_t opd2 = operandStack.top();
        operandStack.pop();
        uint64_t opd1 = operandStack.top();
        operandStack.pop();
        op->appendOperand(opd1);
        op->appendOperand(opd2);
        operandStack.push(op->eval());
        break;
      }
      case Operator::Ternary: {
        uint64_t opd3 = operandStack.top();
        operandStack.pop();
        uint64_t opd2 = operandStack.top();
        operandStack.pop();
        uint64_t opd1 = operandStack.top();
        operandStack.pop();
        op->appendOperand(opd1);
        op->appendOperand(opd2);
        op->appendOperand(opd3);
        operandStack.push(op->eval());
        break;
      }
      default:
        assert(0 && "Unsupport operator!");
        break;
      }
      break;
    }

    case ExprToken::OPERAND: {
      Operand* opd = llvm::cast<Operand>(*it);
      switch (opd->type()) {
      case Operand::SYMBOL: {
        const LDSymbol* symbol =
          m_Module.getNamePool().findSymbol(opd->strVal());
        if (symbol == NULL) {
          fatal(diag::fail_sym_resolution) << __FILE__ << __LINE__
                                           << "mclinker@googlegroups.com";
        }
        operandStack.push(symbol->value());
        break;
      }
      case Operand::INTEGER:
        operandStack.push(opd->intVal());
        break;
      case Operand::SECTION:
      case Operand::DOT:
      default:
        assert(0 && "Unsupport operand!");
        break;
      }
      break;
    }

    } // end of switch
  } // end of for
  // stack top is result
  pResult = operandStack.top();
  return true;
}

