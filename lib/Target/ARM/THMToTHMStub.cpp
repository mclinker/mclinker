//===- THMToTHMStub.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "THMToTHMStub.h"
#include "ARMLDBackend.h"

#include <llvm/Support/ELF.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/Fragment/Relocation.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// THMToTHMStub
//===----------------------------------------------------------------------===//
const uint32_t THMToTHMStub::PIC_TEMPLATE[] = {
  0x46c04778, // bx    pc ... nop
  0xe59fc004, // ldr   r12, [pc, #4]
  0xe08fc00c, // add   ip, pc, ip
  0xe12fff1c, // bx    ip
  0x0         // dcd   R_ARM_REL32(X)
};

const uint32_t THMToTHMStub::TEMPLATE[] = {
  0x46c04778, // bx    pc ... nop
  0xe59fc000, // ldr   ip, [pc, #0]
  0xe12fff1c, // bx    ip
  0x0         // dcd   R_ARM_ABS32(X)
};

THMToTHMStub::THMToTHMStub()
 : Stub(), m_Name("T2T_prototype"), m_pData(NULL), m_Size(0x0)
{
}

THMToTHMStub::THMToTHMStub(bool pIsOutputPIC)
 : Stub(), m_Name("T2T_veneer"), m_pData(NULL), m_Size(0x0)
{
  if (pIsOutputPIC) {
    m_Size = sizeof(PIC_TEMPLATE);
    m_pData = new uint32_t[m_Size];
    std::memcpy(m_pData, PIC_TEMPLATE, m_Size);
    addFixup(16u, 0x0, llvm::ELF::R_ARM_REL32);
  }
  else {
    m_Size = sizeof(TEMPLATE);
    m_pData = new uint32_t[m_Size];
    std::memcpy(m_pData, TEMPLATE, m_Size);
    addFixup(12u, 0x0, llvm::ELF::R_ARM_ABS32);
  }
}

THMToTHMStub::~THMToTHMStub()
{
  if (NULL != m_pData)
    delete[] m_pData;
}

bool THMToTHMStub::isMyDuty(const class Relocation& pReloc,
                            uint64_t pSource,
                            uint64_t pTargetSymValue) const
{
  bool result = false;
  switch (pReloc.type()) {
    case llvm::ELF::R_ARM_THM_CALL:
    case llvm::ELF::R_ARM_THM_JUMP24: {
      if ((pTargetSymValue & 0x1) == 0x0)
        break;

      uint64_t dest = pTargetSymValue + pReloc.addend() + 4u;
      int64_t branch_offset = static_cast<int64_t>(dest) - pSource;
      if ((branch_offset > ARMGNULDBackend::THM_MAX_FWD_BRANCH_OFFSET) ||
          (branch_offset < ARMGNULDBackend::THM_MAX_BWD_BRANCH_OFFSET))
        result =  true;
      break;
    }
    default:
      break;
  }
  return result;
}

const std::string& THMToTHMStub::name() const
{
  return m_Name;
}

uint8_t* THMToTHMStub::getContent()
{
  return reinterpret_cast<uint8_t*>(m_pData);
}

const uint8_t* THMToTHMStub::getContent() const
{
  return reinterpret_cast<const uint8_t*>(m_pData);
}

size_t THMToTHMStub::size() const
{
  return m_Size;
}

size_t THMToTHMStub::alignment() const
{
  return 4u;
}

uint64_t THMToTHMStub::initSymValue() const
{
  return 0x1;
}

Stub* THMToTHMStub::doClone(bool pIsOutputPIC)
{
  return new THMToTHMStub(pIsOutputPIC);
}

