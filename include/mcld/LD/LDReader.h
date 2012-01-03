//===- LDReader.h ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_READER_INTERFACE_H
#define MCLD_READER_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/Support/DataTypes.h>

namespace mcld
{

class Input;

/** \class LDReader
 *  \brief LDReader provides the basic interfaces for all readers. It also
 *  provides basic functions to read data stream.
 */
class LDReader
{
public:
  enum Endian {
    LittleEndian,
    BigEndian
  };

protected:
  LDReader() { }

public:
  virtual ~LDReader() { }

  virtual bool isMyFormat(Input& pInput) const = 0;

  virtual Endian endian(Input& pFile) const = 0;

  /// bswap16 - byte swap 16-bit version
  /// @ref binary utilities - elfcpp_swap
  inline uint16_t bswap16(uint16_t pData) const {
    return ((pData >> 8) & 0xFF) | ((pData & 0xFF) << 8);
  }

  /// bswap32 - byte swap 32-bit version
  /// @ref elfcpp_swap
  inline uint32_t bswap32(uint32_t pData) const {
    return (((pData & 0xFF000000) >> 24) |
            ((pData & 0x00FF0000) >>  8) |
            ((pData & 0x0000FF00) <<  8) |
            ((pData & 0x000000FF) << 24));

  }

  /// bswap64 - byte swap 64-bit version
  /// @ref binary utilities - elfcpp_swap
  inline uint64_t bswap64(uint64_t pData) const {
  return (((pData & 0xFF00000000000000ULL) >> 56) |
          ((pData & 0x00FF000000000000ULL) >> 40) |
          ((pData & 0x0000FF0000000000ULL) >> 24) |
          ((pData & 0x000000FF00000000ULL) >>  8) |
          ((pData & 0x00000000FF000000ULL) <<  8) |
          ((pData & 0x0000000000FF0000ULL) << 24) |
          ((pData & 0x000000000000FF00ULL) << 40) |
          ((pData & 0x00000000000000FFULL) << 56));
  }

};

} // namespace of mcld

#endif

