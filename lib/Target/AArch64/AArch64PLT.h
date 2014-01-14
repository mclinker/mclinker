//===- AArch64PLT.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_AARCH64_PLT_H
#define MCLD_AARCH64_PLT_H

#include <mcld/Target/GOT.h>
#include <mcld/Target/PLT.h>
#include <mcld/Support/MemoryRegion.h>

namespace {

const uint32_t aarch64_plt0[] = {
  // FIXME
  0x0
};

const uint32_t aarch64_plt1[] = {
  // FIXME
  0x0
};

} // anonymous namespace

namespace mcld {

class AArch64GOT;

class AArch64PLT0 : public PLT::Entry<sizeof(aarch64_plt0)>
{
public:
  AArch64PLT0(SectionData& pParent);
};

class AArch64PLT1 : public PLT::Entry<sizeof(aarch64_plt1)>
{
public:
  AArch64PLT1(SectionData& pParent);
};

/** \class AArch64PLT
 *  \brief AArch64 Procedure Linkage Table
 */
class AArch64PLT : public PLT
{
public:
  AArch64PLT(LDSection& pSection, AArch64GOT& pGOTPLT);
  ~AArch64PLT();

  // finalizeSectionSize - set LDSection size
  void finalizeSectionSize();

  // hasPLT1 - return if this plt section has any plt1 entry
  bool hasPLT1() const;

  AArch64PLT1* create();

  AArch64PLT0* getPLT0() const;

  void applyPLT0();

  void applyPLT1();

  uint64_t emit(MemoryRegion& pRegion);

private:
  AArch64GOT& m_GOT;
};

} // namespace of mcld

#endif

