//===- HexagonRelocator.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/ADT/Twine.h>
#include <llvm/Support/DataTypes.h>
#include <llvm/Support/ELF.h>
#include <mcld/Support/MsgHandling.h>

#include "HexagonRelocator.h"
#include "HexagonRelocationFunctions.h"

using namespace mcld;

//===--------------------------------------------------------------------===//
// Relocation Functions and Tables
//===--------------------------------------------------------------------===//
DECL_HEXAGON_APPLY_RELOC_FUNCS

/// the prototype of applying function
typedef Relocator::Result (*ApplyFunctionType)(Relocation& pReloc,
                                               HexagonRelocator& pParent);

// the table entry of applying functions
struct ApplyFunctionTriple
{
  ApplyFunctionType func;
  unsigned int type;
  const char* name;
};

// declare the table of applying functions
static const ApplyFunctionTriple ApplyFunctions[] = {
  DECL_HEXAGON_APPLY_RELOC_FUNC_PTRS
};

//===--------------------------------------------------------------------===//
// HexagonRelocator
//===--------------------------------------------------------------------===//
HexagonRelocator::HexagonRelocator(HexagonLDBackend& pParent,
                                   const LinkerConfig& pConfig)
  : Relocator(pConfig),
    m_Target(pParent) {
}

HexagonRelocator::~HexagonRelocator()
{
}

Relocator::Result
HexagonRelocator::applyRelocation(Relocation& pRelocation)
{
  Relocation::Type type = pRelocation.type();

  if (type > 85) { // 86-255 relocs do not exists for Hexagon
    return Relocator::Unknown;
  }

  // apply the relocation
  return ApplyFunctions[type].func(pRelocation, *this);
}

const char* HexagonRelocator::getName(Relocation::Type pType) const
{
  return ApplyFunctions[pType].name;
}

Relocator::Size HexagonRelocator::getSize(Relocation::Type pType) const
{
  return 32;
}

void HexagonRelocator::scanRelocation(Relocation& pReloc,
                                      IRBuilder& pBuilder,
                                      Module& pModule,
                                      LDSection& pSection)
{
  pReloc.updateAddend();
}

//===--------------------------------------------------------------------===//
// Relocation helper function
//===--------------------------------------------------------------------===//
template<typename T1, typename T2>
T1 ApplyMask(T2 pMask, T1 pData) {
  T1 result = 0;
  size_t off = 0;

  for (size_t bit = 0; bit != sizeof (T1) * 8; ++bit) {
    const bool valBit = (pData >> off) & 1;
    const bool maskBit = (pMask >> bit) & 1;
    if (maskBit) {
      result |= static_cast<T1>(valBit) << bit;
      ++off;
    }
  }
  return result;
}

//=========================================//
// Each relocation function implementation //
//=========================================//

// R_HEX_NONE
HexagonRelocator::Result none(Relocation& pReloc, HexagonRelocator& pParent)
{
  return HexagonRelocator::OK;
}

// R_HEX_B15_PCREL: Word32_B15 : 0x00df20fe  (S + A - P) >> 2 : Signed Verify
HexagonRelocator::Result relocB15PCREL(Relocation& pReloc,
                                       HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();
  HexagonRelocator::DWord   P = pReloc.place();

  int32_t result = (int32_t) ((S + A - P) >> 2);
  int32_t range =  1 << 14;
  if ( (result < range) && (result > -range)) {
    pReloc.target() = pReloc.target() | ApplyMask<int32_t>(0x00df20fe,result);
    return HexagonRelocator::OK;
  }
  return HexagonRelocator::Overflow;
}

// R_HEX_B22_PCREL: Word32_B22 : 0x01ff3ffe  (S + A - P) >> 2 : Signed Verify
HexagonRelocator::Result relocB22PCREL(Relocation& pReloc,
                                       HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();
  HexagonRelocator::DWord   P = pReloc.place();

  int32_t result = (int32_t) ((S + A - P) >> 2);
  int32_t range = 1 << 21;
  uint32_t bitMask = FINDBITMASK(pReloc.target());
  if ( (result < range) && (result > -range)) {
    pReloc.target() = pReloc.target() | ApplyMask<int32_t>(bitMask, result);
    return HexagonRelocator::OK;
  }
  return HexagonRelocator::Overflow;
}

// R_HEX_B7_PCREL: Word32_B7 : 0x0001f18  (S + A - P) >> 2 : Signed Verify
HexagonRelocator::Result relocB7PCREL(Relocation& pReloc,
                                      HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();
  HexagonRelocator::DWord   P = pReloc.place();

  int32_t result = (int32_t) ((S + A - P) >> 2);
  int32_t range = 1 << 6;
  if ( (result < range) && (result > -range)) {
    pReloc.target() = pReloc.target() | ApplyMask<int32_t>(0x00001f18, result);
    return HexagonRelocator::OK;
  }
  return HexagonRelocator::Overflow;
}

// R_HEX_32: Word32 : 0xffffffff : (S + A) : Unsigned Truncate
HexagonRelocator::Result reloc32(Relocation& pReloc,
                                 HexagonRelocator& pParent)
{
  HexagonRelocator::DWord A = pReloc.addend();
  HexagonRelocator::DWord S = pReloc.symValue();

  uint32_t result = (uint32_t) (S + A);

  pReloc.target() = result | pReloc.target();
  return HexagonRelocator::OK;
}

// R_HEX_16: Word32 : 0xffff : (S + A) : Unsigned Truncate
HexagonRelocator::Result reloc16(Relocation& pReloc,
                                 HexagonRelocator& pParent)
{
  HexagonRelocator::DWord A = pReloc.addend();
  HexagonRelocator::DWord S = pReloc.symValue();

  uint32_t result = (uint32_t) (S + A);
  pReloc.target() = pReloc.target() | ApplyMask<uint32_t>(0x0000ffff, result);

  return HexagonRelocator::OK;
}

// R_HEX_8: Word32 : 0xff : (S + A) : Unsigned Truncate
HexagonRelocator::Result reloc8(Relocation& pReloc,
                                HexagonRelocator& pParent)
{
  HexagonRelocator::DWord A = pReloc.addend();
  HexagonRelocator::DWord S = pReloc.symValue();

  uint32_t result = (uint32_t) (S + A);
  pReloc.target() = pReloc.target() | ApplyMask<uint32_t>(0x000000ff, result);

  return HexagonRelocator::OK;
}

// R_HEX_LO16: Word32_LO : 0x00c03fff  (S + A) : Unsigned Truncate
HexagonRelocator::Result relocLO16(Relocation& pReloc,
                                   HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();

  uint32_t result = (uint32_t) (S + A);
//  result = ((result & 0x3fff) | ((result << 6) & 0x00c00000));
  pReloc.target() = pReloc.target() | ApplyMask<uint32_t>(0x00c03fff, result);
  return HexagonRelocator::OK;
}

// R_HEX_HI16: Word32_LO : 0x00c03fff  (S + A) >> 16 : Unsigned Truncate
HexagonRelocator::Result relocHI16(Relocation& pReloc,
                                   HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();

  uint32_t result = (uint32_t) ((S + A) >> 16);
//  result = ((result & 0x3fff) | ((result << 6) & 0x00c00000));
  pReloc.target() = pReloc.target() | ApplyMask<uint32_t>(0x00c03fff, result);
  return HexagonRelocator::OK;
}

// R_HEX_GPREL16_0 : Word32_GP : 0x061f2ff  (S + A - GP) : Unsigned Verify
HexagonRelocator::Result relocGPREL16_0(Relocation& pReloc,
                                        HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();
  HexagonLDBackend& ld_backend = pParent.getTarget();
  HexagonRelocator::DWord   GP = ld_backend.getGP();

  int64_t result = (int64_t) (S + A - GP);
  int64_t range = 1ULL << 32;
  uint32_t bitMask = FINDBITMASK(pReloc.target());
  if (result <= range) {
    pReloc.target() = pReloc.target() | ApplyMask<uint32_t>(bitMask, result);
    return HexagonRelocator::OK;
  }
  return HexagonRelocator::Overflow;
}

// R_HEX_GPREL16_1 : Word32_GP : 0x061f2ff  (S + A - GP)>>1 : Unsigned Verify
HexagonRelocator::Result relocGPREL16_1(Relocation& pReloc,
                                        HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();
  HexagonLDBackend& ld_backend = pParent.getTarget();
  HexagonRelocator::DWord   GP = ld_backend.getGP();

  int64_t result = (int64_t) ((S + A - GP) >> 1);
  int64_t range = 1LL << 32;
  uint32_t bitMask = FINDBITMASK(pReloc.target());
  if (result <= range) {
    pReloc.target() = pReloc.target() | ApplyMask<uint32_t>(bitMask, result);
    return HexagonRelocator::OK;
  }
  return HexagonRelocator::Overflow;
}

// R_HEX_GPREL16_2 : Word32_GP : 0x061f2ff  (S + A - GP)>>2 : Unsigned Verify
HexagonRelocator::Result relocGPREL16_2(Relocation& pReloc,
                                        HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();
  HexagonLDBackend& ld_backend = pParent.getTarget();
  HexagonRelocator::DWord   GP = ld_backend.getGP();

  int64_t result = (int64_t) ((S + A - GP) >> 2);
  int64_t range = 1LL << 32;
  uint32_t bitMask = FINDBITMASK(pReloc.target());
  if (result <= range) {
    pReloc.target() = pReloc.target() | ApplyMask<uint32_t>(bitMask, result);
    return HexagonRelocator::OK;
  }
  return HexagonRelocator::Overflow;
}

// R_HEX_GPREL16_3 : Word32_GP : 0x061f2ff  (S + A - GP)>>3 : Unsigned Verify
HexagonRelocator::Result relocGPREL16_3(Relocation& pReloc,
                                        HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();
  HexagonLDBackend& ld_backend = pParent.getTarget();
  HexagonRelocator::DWord   GP = ld_backend.getGP();

  int64_t result = (int64_t) ((S + A - GP) >> 3);
  int64_t range = 1LL << 32;
  uint32_t bitMask = FINDBITMASK(pReloc.target());
  if (result <= range) {
    pReloc.target() = pReloc.target() | ApplyMask<uint32_t>(bitMask, result);
    return HexagonRelocator::OK;
  }
  return HexagonRelocator::Overflow;
}

// R_HEX_B13_PCREL : Word32_B13 : 0x00202ffe  (S + A - P)>>2 : Signed Verify
HexagonRelocator::Result relocB13PCREL(Relocation& pReloc,
                                       HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();
  HexagonRelocator::DWord   P = pReloc.place();

  int32_t result = ((S + A - P) >> 2);
  int32_t range = 1L << 12;
  if (result < range && result > -range) {
    pReloc.target() = pReloc.target() | ApplyMask<uint32_t>(0x00202ffe, result);
    return HexagonRelocator::OK;
  }
  return HexagonRelocator::Overflow;
}

// R_HEX_B9_PCREL : Word32_B9 : 0x00300ffe  (S + A - P)>>2 : Signed Verify
HexagonRelocator::Result relocB9PCREL(Relocation& pReloc,
                                      HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();
  HexagonRelocator::DWord   P = pReloc.place();

  int32_t result = ((S + A - P) >> 2);
  int32_t range = 1L << 8;
  uint32_t bitMask = FINDBITMASK(pReloc.target());
  if (result < range && result > -range) {
    pReloc.target() = pReloc.target() | ApplyMask<uint32_t>(bitMask, result);
    return HexagonRelocator::OK;
  }
  return HexagonRelocator::Overflow;
}

// R_HEX_B32_PCREL_X : Word32_X26 : 0x0fff3fff  (S + A - P)>>6 : Truncate
HexagonRelocator::Result relocB32PCRELX(Relocation& pReloc,
                                        HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();
  HexagonRelocator::DWord   P = pReloc.place();

  int32_t result = ((S + A - P) >> 6);
  pReloc.target() = pReloc.target() | ApplyMask<int32_t>(0xfff3fff, result);

  return HexagonRelocator::OK;
}

// R_HEX_32_6_X : Word32_X26 : 0x0fff3fff  (S + A)>>6 : Unsigned Verify
HexagonRelocator::Result reloc32_6_X(Relocation& pReloc,
                                     HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();

  int64_t result = ((S + A) >> 6);
  int64_t range = 1LL << 32;

  if (result > range)
    return HexagonRelocator::Overflow;

  pReloc.target() = pReloc.target() | ApplyMask<uint32_t>(0xfff3fff, result);

  return HexagonRelocator::OK;
}

// R_HEX_B22_PCREL_X : Word32_B22 : 0x01ff3ffe
// ((S + A - P) & 0x3f)>>2 : Signed Verify
HexagonRelocator::Result relocB22PCRELX(Relocation& pReloc,
                                        HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();
  HexagonRelocator::DWord   P = pReloc.place();

  int32_t result = ((S + A - P) & 0x3f);
  int32_t range = 1 << 21;

  if (result < range && result > -range)  {
    pReloc.target() = pReloc.target() | ApplyMask<uint32_t>(0x01ff3ffe, result);
    return HexagonRelocator::OK;
  }

  return HexagonRelocator::Overflow;
}

// R_HEX_B15_PCREL_X : Word32_B15 : 0x00df20fe
// ((S + A - P) & 0x3f)>>2 : Signed Verify
HexagonRelocator::Result relocB15PCRELX(Relocation& pReloc,
                                        HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();
  HexagonRelocator::DWord   P = pReloc.place();

  int32_t result = ((S + A - P) & 0x3f);
  int32_t range = 1 << 14;

  if (result < range && result > -range)  {
    pReloc.target() = pReloc.target() | ApplyMask<int32_t>(0x00df20fe, result);
    return HexagonRelocator::OK;
  }

  return HexagonRelocator::Overflow;
}

// R_HEX_B13_PCREL_X : Word32_B13 : 0x00202ffe
// ((S + A - P) & 0x3f)>>2 : Signed Verify
HexagonRelocator::Result relocB13PCRELX(Relocation& pReloc,
                                        HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();
  HexagonRelocator::DWord   P = pReloc.place();

  int32_t result = ((S + A - P) & 0x3f);
  int32_t range = 1 << 12;

  if (result < range && result > -range)  {
    pReloc.target() = pReloc.target() | ApplyMask<int32_t>(0x00202ffe, result);
    return HexagonRelocator::OK;
  }

  return HexagonRelocator::Overflow;
}

// R_HEX_B9_PCREL_X : Word32_B9 : 0x003000fe
// ((S + A - P) & 0x3f)>>2 : Signed Verify
HexagonRelocator::Result relocB9PCRELX(Relocation& pReloc,
                                       HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();
  HexagonRelocator::DWord   P = pReloc.place();

  int32_t result = ((S + A - P) & 0x3f);
  int32_t range = 1 << 8;

  uint32_t bitMask = FINDBITMASK(pReloc.target());
  if (result < range && result > -range)  {
    pReloc.target() = pReloc.target() | ApplyMask<int32_t>(bitMask, result);
    return HexagonRelocator::OK;
  }

  return HexagonRelocator::Overflow;
}

// R_HEX_B7_PCREL_X : Word32_B7 : 0x00001f18
// ((S + A - P) & 0x3f)>>2 : Signed Verify
HexagonRelocator::Result relocB7PCRELX(Relocation& pReloc,
                                       HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();
  HexagonRelocator::DWord   P = pReloc.place();

  int32_t result = ((S + A - P) & 0x3f);
  int32_t range = 1 << 6;

  if (result < range && result > -range)  {
    pReloc.target() = pReloc.target() | ApplyMask<int32_t>(0x00001f18, result);
    return HexagonRelocator::OK;
  }

  return HexagonRelocator::Overflow;
}

// R_HEX_32_PCREL : Word32 : 0xffffffff  (S + A - P) : Signed Verify
HexagonRelocator::Result reloc32PCREL(Relocation& pReloc,
                                      HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();
  HexagonRelocator::DWord   P = pReloc.place();

  int64_t result = S + A - P;
  int32_t range = 1 << 31;

  if (result < range && result > -range)  {
    pReloc.target() = pReloc.target() | ApplyMask<int32_t>(0xffffffff, result);
    return HexagonRelocator::OK;
  }

  return HexagonRelocator::Overflow;
}

// R_HEX_N_X : Word32_U6 : (S + A) : Unsigned Truncate
HexagonRelocator::Result relocHexNX(Relocation& pReloc,
                                    HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();
  uint32_t result = (S + A);
  uint32_t bitMask = FINDBITMASK(pReloc.target());

  pReloc.target() = pReloc.target() | ApplyMask<uint32_t>(bitMask, result);
  return HexagonRelocator::OK;
}

HexagonRelocator::Result unsupport(Relocation& pReloc,
                                   HexagonRelocator& pParent)
{
  return HexagonRelocator::Unsupport;
}
