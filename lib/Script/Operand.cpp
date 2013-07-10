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

//===----------------------------------------------------------------------===//
// Operand
//===----------------------------------------------------------------------===//
Operand::Operand(Type pType)
  : ExprToken(ExprToken::OPERAND), m_Type(pType)
{
}

Operand::~Operand()
{
}

//===----------------------------------------------------------------------===//
// SymOperand
//===----------------------------------------------------------------------===//
typedef GCFactory<SymOperand, MCLD_SYMBOLS_PER_INPUT> SymOperandFactory;
static llvm::ManagedStatic<SymOperandFactory> g_SymOperandFactory;

SymOperand::SymOperand()
  : Operand(Operand::SYMBOL)
{
}

SymOperand::SymOperand(const std::string& pName)
  : Operand(Operand::SYMBOL), m_Name(pName)
{
}

void SymOperand::dump() const
{
  mcld::outs() << m_Name;
}

bool SymOperand::isDot() const
{
  assert(!m_Name.empty());
  return m_Name.size() == 1 && m_Name[0] == '.';
}

SymOperand* SymOperand::create(const std::string& pName)
{
  SymOperand* result = g_SymOperandFactory->allocate();
  new (result) SymOperand(pName);
  return result;
}

void SymOperand::destroy(SymOperand*& pOperand)
{
  g_SymOperandFactory->destroy(pOperand);
  g_SymOperandFactory->deallocate(pOperand);
  pOperand = NULL;
}

void SymOperand::clear()
{
  g_SymOperandFactory->clear();
}

//===----------------------------------------------------------------------===//
// IntOperand
//===----------------------------------------------------------------------===//
typedef GCFactory<IntOperand, MCLD_SYMBOLS_PER_INPUT> IntOperandFactory;
static llvm::ManagedStatic<IntOperandFactory> g_IntOperandFactory;

IntOperand::IntOperand()
  : Operand(Operand::INTEGER)
{
}

IntOperand::IntOperand(uint64_t pValue)
  : Operand(Operand::INTEGER), m_Value(pValue)
{
}

void IntOperand::dump() const
{
  mcld::outs() << m_Value;
}

IntOperand* IntOperand::create(uint64_t pValue)
{
  IntOperand* result = g_IntOperandFactory->allocate();
  new (result) IntOperand(pValue);
  return result;
}

void IntOperand::destroy(IntOperand*& pOperand)
{
  g_IntOperandFactory->destroy(pOperand);
  g_IntOperandFactory->deallocate(pOperand);
  pOperand = NULL;
}

void IntOperand::clear()
{
  g_IntOperandFactory->clear();
}
