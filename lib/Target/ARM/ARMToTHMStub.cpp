//===- ARMToTHMStub.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "ARMToTHMStub.h"
#include "ARMLDBackend.h"

#include <llvm/Support/ELF.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/Fragment/Relocation.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// ARMToTHMStub
//===----------------------------------------------------------------------===//
const uint32_t ARMToTHMStub::PIC_TEMPLATE[] = {
  0xe59fc004, // ldr   r12, [pc, #4]
  0xe08fc00c, // add   ip, pc, ip
  0xe12fff1c, // bx    ip
  0x0         // dcd   R_ARM_REL32(X)
};

const uint32_t ARMToTHMStub::TEMPLATE[] = {
  0xe59fc000, // ldr   ip, [pc, #0]
  0xe12fff1c, // bx    ip
  0x0         // dcd   R_ARM_ABS32(X)
};

ARMToTHMStub::ARMToTHMStub()
 : Stub(), m_Name("A2T_prototype"), m_pData(NULL), m_Size(0x0)
{
}

ARMToTHMStub::ARMToTHMStub(bool pIsOutputPIC)
 : Stub(), m_Name("A2T_veneer"), m_pData(NULL), m_Size(0x0)
{
  if (pIsOutputPIC) {
    m_Size = sizeof(PIC_TEMPLATE);
    m_pData = new uint32_t[m_Size];
    std::memcpy(m_pData, PIC_TEMPLATE, m_Size);
    addFixup(12u, 0x0, llvm::ELF::R_ARM_REL32);
  }
  else {
    m_Size = sizeof(TEMPLATE);
    m_pData = new uint32_t[m_Size];
    std::memcpy(m_pData, TEMPLATE, m_Size);
    addFixup(8u, 0x0, llvm::ELF::R_ARM_ABS32);
  }
}

ARMToTHMStub::~ARMToTHMStub()
{
  if (NULL != m_pData)
    delete[] m_pData;
}

bool ARMToTHMStub::isMyDuty(const class Relocation& pReloc,
                            uint64_t pSource,
                            uint64_t pTargetSymValue) const
{
  bool result = false;
  switch (pReloc.type()) {
    case llvm::ELF::R_ARM_CALL: {
      // check the T bit of the target
      if ((pTargetSymValue & 0x1) == 0x0)
        break;

      if ((pReloc.symInfo()->outSymbol()->fragRef() != NULL)) { 
        const Fragment* frag = pReloc.symInfo()->outSymbol()->fragRef()->frag();
        if (classof(frag)) {
          result = false;
          break;
        }
      }

      uint64_t dest = pTargetSymValue + pReloc.addend() + 8u;
      int64_t branch_offset = static_cast<int64_t>(dest) - pSource;
      if ((branch_offset > ARMGNULDBackend::ARM_MAX_FWD_BRANCH_OFFSET) ||
          (branch_offset < ARMGNULDBackend::ARM_MAX_BWD_BRANCH_OFFSET)) {
        result = true;
        break;
      }
      break;
    }
    case llvm::ELF::R_ARM_JUMP24:
    case llvm::ELF::R_ARM_PLT32: {
      if ((pTargetSymValue & 0x1) == 0x0)
        break;

      if ((pReloc.symInfo()->outSymbol()->fragRef() != NULL)) { 
        const Fragment* frag = pReloc.symInfo()->outSymbol()->fragRef()->frag();
        if (classof(frag)) {
          result = false;
          break;
        }
      }
      result = true;
      break;
    }
    default:
      break;
  }
  return result;
}

const std::string& ARMToTHMStub::name() const
{
  return m_Name;
}

uint8_t* ARMToTHMStub::getContent()
{
  return reinterpret_cast<uint8_t*>(m_pData);
}

const uint8_t* ARMToTHMStub::getContent() const
{
  return reinterpret_cast<const uint8_t*>(m_pData);
}

size_t ARMToTHMStub::size() const
{
  return m_Size;
}

size_t ARMToTHMStub::alignment() const
{
  return 4u;
}

Stub* ARMToTHMStub::doClone(bool pIsOutputPIC)
{
  return new ARMToTHMStub(pIsOutputPIC);
}

