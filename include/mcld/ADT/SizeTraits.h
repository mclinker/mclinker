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

template<size_t SIZE>
class SizeTraits;

template<>
class SizeTraits<32>
{
public:
  typedef uint32_t Address;
  typedef uint32_t Offset;
  typedef uint32_t Word;
  typedef int32_t  SWord;
};

template<>
class SizeTraits<64>
{
public:
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

/// bswap16 - byte swap 16-bit version
/// @ref binary utilities - elfcpp_swap
inline uint16_t bswap16(uint16_t pData)
{
   return ((pData >> 8) & 0xFF) | ((pData & 0xFF) << 8);
}

/// bswap32 - byte swap 32-bit version
/// @ref elfcpp_swap
inline uint32_t bswap32(uint32_t pData)
{
   return (((pData & 0xFF000000) >> 24) |
           ((pData & 0x00FF0000) >>  8) |
           ((pData & 0x0000FF00) <<  8) |
           ((pData & 0x000000FF) << 24));

}

/// bswap64 - byte swap 64-bit version
/// @ref binary utilities - elfcpp_swap
inline uint64_t bswap64(uint64_t pData)
{
   return (((pData & 0xFF00000000000000ULL) >> 56) |
           ((pData & 0x00FF000000000000ULL) >> 40) |
           ((pData & 0x0000FF0000000000ULL) >> 24) |
           ((pData & 0x000000FF00000000ULL) >>  8) |
           ((pData & 0x00000000FF000000ULL) <<  8) |
           ((pData & 0x0000000000FF0000ULL) << 24) |
           ((pData & 0x000000000000FF00ULL) << 40) |
           ((pData & 0x00000000000000FFULL) << 56));
}

template <size_t SizeOfStr, typename FieldType>
class StringSizerHelper
{
private:
  char FIELD_TOO_SMALL[SizeOfStr <= FieldType(~0U) ? 1 : -1];
public:
  enum { Size = SizeOfStr };
};

#define STR_SIZE(str, fieldTy) StringSizerHelper<sizeof(str)-1, fieldTy>::Size

} // namespace of mcld

#endif

