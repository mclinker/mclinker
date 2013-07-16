//===- BinaryOp.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/BinaryOp.h>
#include <mcld/Script/Operand.h>
#include <mcld/ADT/SizeTraits.h>
#include <mcld/Module.h>
#include <mcld/LinkerScript.h>
#include <llvm/Support/Casting.h>
#include <cassert>

using namespace mcld;
//===----------------------------------------------------------------------===//
// BinaryOp
//===----------------------------------------------------------------------===//
template<>
IntOperand* BinaryOp<Operator::MUL>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() * m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::DIV>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() / m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::MOD>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() % m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::ADD>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() + m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::SUB>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() - m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::LSHIFT>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() << m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::RSHIFT>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() >> m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::LT>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() < m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::LE>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() <= m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::GT>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() > m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::GE>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() >= m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::EQ>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() == m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::NE>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() != m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::BITWISE_AND>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() & m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::BITWISE_XOR>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() ^ m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::BITWISE_OR>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() | m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::LOGICAL_AND>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() && m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::LOGICAL_OR>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() || m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::ALIGN>::eval()
{
  IntOperand* res = result();
  uint64_t value = m_pOperand[0]->value();
  uint64_t align = m_pOperand[1]->value();
  alignAddress(value, align);
  res->setValue(value);
  return res;
}

template<>
IntOperand* BinaryOp<Operator::DATA_SEGMENT_ALIGN>::eval()
{
  // TODO
  assert(0);
  IntOperand* res = result();
  return res;
}

template<>
IntOperand* BinaryOp<Operator::DATA_SEGMENT_RELRO_END>::eval()
{
  // TODO
  assert(0);
  IntOperand* res = result();
  return res;
}

template<>
IntOperand* BinaryOp<Operator::MAX>::eval()
{
  IntOperand* res = result();
  if (m_pOperand[0]->value() >= m_pOperand[1]->value())
    res->setValue(m_pOperand[0]->value());
  else
    res->setValue(m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::MIN>::eval()
{
  IntOperand* res = result();
  if (m_pOperand[0]->value() <= m_pOperand[1]->value())
    res->setValue(m_pOperand[0]->value());
  else
    res->setValue(m_pOperand[1]->value());
  return res;
}


/* SEGMENT_START(segment, default) */
template<>
IntOperand* BinaryOp<Operator::SEGMENT_START>::eval()
{
  IntOperand* res = result();
  /* Currently we look up segment address from -T command line options. */
  SectOperand* sect = llvm::cast<SectOperand>(m_pOperand[0]);
  const LinkerScript::AddressMap& addressMap =
    module().getScript().addressMap();
  LinkerScript::AddressMap::const_iterator addr;
  if (sect->name().compare("text-segment") == 0)
    addr = addressMap.find(".text");
  else if (sect->name().compare("data-segment") == 0)
    addr = addressMap.find(".data");
  else if (sect->name().compare("bss-segment") == 0)
    addr = addressMap.find(".bss");
  else
    addr = addressMap.find(sect->name());

  if (addr != addressMap.end())
    res->setValue(addr.getEntry()->value());
  else {
    assert(m_pOperand[1]->type() == Operand::INTEGER);
    res->setValue(m_pOperand[1]->value());
  }
  return res;
}
