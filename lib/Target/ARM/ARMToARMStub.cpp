//===- ARMToARMStub.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "ARMToARMStub.h"
#include "ARMLDBackend.h"

#include <llvm/Support/ELF.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/Fragment/Relocation.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// ARMToARMStub
//===----------------------------------------------------------------------===//
const uint32_t ARMToARMStub::PIC_TEMPLATE[] = {
  0xe59fc000, // ldr   r12, [pc]
  0xe08ff00c, // add   pc, pc, ip
  0x0         // dcd   R_ARM_REL32(X-4)
};

const uint32_t ARMToARMStub::TEMPLATE[] = {
  0xe51ff004, // ldr   pc, [pc, #-4]
  0x0         // dcd   R_ARM_ABS32(X)
};

ARMToARMStub::ARMToARMStub()
 : Stub(), m_Name("A2A_prototype"), m_pData(NULL), m_Size(0x0)
{
}

ARMToARMStub::ARMToARMStub(bool pIsOutputPIC)
 : Stub(), m_Name("A2A_veneer"), m_pData(NULL), m_Size(0x0)
{
  if (pIsOutputPIC) {
    m_Size = sizeof(PIC_TEMPLATE);
    m_pData = new uint32_t[m_Size];
    std::memcpy(m_pData, PIC_TEMPLATE, m_Size);
    addFixup(8u, -4, llvm::ELF::R_ARM_REL32);
  }
  else {
    m_Size = sizeof(TEMPLATE);
    m_pData = new uint32_t[m_Size];
    std::memcpy(m_pData, TEMPLATE, m_Size);
    addFixup(4u, 0x0, llvm::ELF::R_ARM_ABS32);
  }
}

ARMToARMStub::~ARMToARMStub()
{
  if (NULL != m_pData)
    delete[] m_pData;
}

bool ARMToARMStub::isMyDuty(const class Relocation& pReloc,
                            uint64_t pSource,
                            uint64_t pTargetSymValue) const
{
  bool result = false;
  switch (pReloc.type()) {
    case llvm::ELF::R_ARM_CALL:
    case llvm::ELF::R_ARM_JUMP24:
    case llvm::ELF::R_ARM_PLT32: {
      // check the T bit of the target
      if ((pTargetSymValue & 0x1) != 0x0)
        break;

      // check if the branch target is too far
      uint64_t dest = pTargetSymValue + pReloc.addend() + 8u;
      int64_t branch_offset = static_cast<int64_t>(dest) - pSource;
      if ((branch_offset > ARMGNULDBackend::ARM_MAX_FWD_BRANCH_OFFSET) ||
          (branch_offset < ARMGNULDBackend::ARM_MAX_BWD_BRANCH_OFFSET)) {
        result =  true;
      }
      break;
    }
    default:
      break;
  }
  return result;
}

const std::string& ARMToARMStub::name() const
{
  return m_Name;
}

uint8_t* ARMToARMStub::getContent()
{
  return reinterpret_cast<uint8_t*>(m_pData);
}

const uint8_t* ARMToARMStub::getContent() const
{
  return reinterpret_cast<const uint8_t*>(m_pData);
}

size_t ARMToARMStub::size() const
{
  return m_Size;
}

size_t ARMToARMStub::alignment() const
{
  return 4u;
}

Stub* ARMToARMStub::doClone(bool pIsOutputPIC)
{
  return new ARMToARMStub(pIsOutputPIC);
}

