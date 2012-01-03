//===- SizeTraits.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SIZE_TRAITS_H
#define MCLD_SIZE_TRAITS_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/Support/DataTypes.h>

namespace mcld
{

template<int SIZE>
struct SizeTraits;

template<>
class SizeTraits<32>
{
  typedef uint32_t Address;
  typedef uint32_t Offset;
  typedef uint32_t Word;
  typedef int32_t  SWord;
};

template<>
class SizeTraits<64>
{
  typedef uint64_t Address;
  typedef uint64_t Offset;
  typedef uint64_t Word;
  typedef int64_t  SWord;
};

/// alignAddress - helper function to align an address with given alignment
/// constraint
///
/// @param pAddr - the address to be aligned
/// @param pAlignConstraint - the alignment used to align the given address
inline void alignAddress(uint64_t& pAddr, uint64_t pAlignConstraint)
{
  if (pAlignConstraint != 0)
    pAddr = (pAddr + pAlignConstraint - 1) &~ (pAlignConstraint - 1);
}

template<size_t Constraint>
uint64_t Align(uint64_t pAddress);

template<>
inline uint64_t Align<32>(uint64_t pAddress)
{
  return (pAddress + 0x1F) & (~0x1F);
}

template<>
inline uint64_t Align<64>(uint64_t pAddress)
{
  return (pAddress + 0x3F) & (~0x3F);
}

} // namespace of mcld

#endif

