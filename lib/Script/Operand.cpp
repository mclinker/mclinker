//===- Operand.cpp --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/Operand.h>
#include <mcld/Support/raw_ostream.h>
#include <mcld/Support/GCFactory.h>
#include <llvm/Support/ManagedStatic.h>
#include <cassert>

using namespace mcld;

typedef GCFactory<Operand, MCLD_SYMBOLS_PER_INPUT> OperandFactory;
static llvm::ManagedStatic<OperandFactory> g_OperandFactory;

//===----------------------------------------------------------------------===//
// Operand
//===----------------------------------------------------------------------===//
Operand::Operand()
  : ExprToken(ExprToken::Opd), m_Type(UNKNOWN)
{
}

Operand::Operand(Type pType, const std::string& pValue)
  : ExprToken(ExprToken::Opd), m_Type(pType)
{
  m_Data.strVal = &pValue;
}

Operand::Operand(uint64_t pValue)
  : ExprToken(ExprToken::Opd), m_Type(INTEGER)
{
  m_Data.intVal = pValue;
}

Operand::~Operand()
{
}

void Operand::dump() const
{
  switch (type()) {
  case SYMBOL:
  case SECTION:
  case DOT:
    mcld::outs() << *(m_Data.strVal) << " ";
    break;
  case INTEGER:
    mcld::outs() << m_Data.intVal << " ";
    break;
  case UNKNOWN:
  default:
    assert(0 && "Invalid Operand!");
    break;
  }
}

Operand* Operand::create(uint64_t pValue)
{
  Operand* result = g_OperandFactory->allocate();
  new (result) Operand(pValue);
  return result;
}

Operand* Operand::create(Type pType, const std::string& pValue)
{
  Operand* result = g_OperandFactory->allocate();
  new (result) Operand(pType, pValue);
  return result;
}

void Operand::destroy(Operand*& pOperand)
{
  g_OperandFactory->destroy(pOperand);
  g_OperandFactory->deallocate(pOperand);
  pOperand = NULL;
}

void Operand::clear()
{
  g_OperandFactory->clear();
}

