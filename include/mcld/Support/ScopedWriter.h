//===- ScopedWriter.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_SCOPED_REGION_WRITER_H
#define MCLD_SCOPED_REGION_WRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Support/MemoryRegion.h>
#include <llvm/Support/DataTypes.h>

namespace mcld
{

#define WRITE_SAME_DATA(TYPE) void WriteSame##TYPE ( TYPE pValue ) { \
    if(m_Cursor+sizeof(TYPE) <= m_Region->size()) { \
      *(( TYPE *)(m_Buffer+m_Cursor)) = pValue; \
      m_Cursor += sizeof(TYPE); \
    } \
    else \
      llvm::report_fatal_error("Memory access is out of boundary!"); \
}

/** \class ScopedWriter
 *  \brief ScopedWriter is a region writer that is automaitcally synchronized
 *  at destruction.
 *
 *  ScopedWriter writes host data to target region. It also amends the endian
 *  from host to target.
 */
class ScopedWriter
{
public:
  typedef MemoryRegion::Address Address;

public:
  ScopedWriter(MemoryRegion *pRegion, bool pSameEndian);

  ~ScopedWriter()
  { m_Region->sync(); }

  void Write16(uint16_t pValue) {
    if (m_SameEndian)
      WriteSameuint16_t(pValue);
    else
      WriteOpposite16(pValue);
  }

  void Write32(uint32_t pValue) {
    if (m_SameEndian)
      WriteSameuint32_t(pValue);
    else
      WriteOpposite32(pValue);
  }

  void Write64(uint64_t pValue) {
    if (m_SameEndian)
      WriteSameuint64_t(pValue);
    else
      WriteOpposite64(pValue);
  }

private:
  WRITE_SAME_DATA(uint8_t)
  WRITE_SAME_DATA(uint16_t)
  WRITE_SAME_DATA(uint32_t)
  WRITE_SAME_DATA(uint64_t)


  void WriteOpposite16(uint16_t pValue) {
    if(m_Cursor+sizeof(uint16_t) <= m_Region->size()) {
      m_Buffer[m_Cursor] = (uint8_t)(pValue >> 8);
      m_Buffer[m_Cursor+1] = (uint8_t)pValue;
    } 
    else 
      llvm::report_fatal_error("Memory access is out of boundary!"); 
  }

  void WriteOpposite32(uint32_t pValue) {
    if(m_Cursor+sizeof(uint32_t) <= m_Region->size()) {
      m_Buffer[m_Cursor]   = (uint8_t)(pValue >> 24);
      m_Buffer[m_Cursor+1] = (uint8_t)(pValue >> 16);
      m_Buffer[m_Cursor+2] = (uint8_t)(pValue >> 8);
      m_Buffer[m_Cursor+3] = (uint8_t)pValue;
    } 
    else 
      llvm::report_fatal_error("Memory access is out of boundary!"); 
  }

  void WriteOpposite64(uint64_t pValue) {
    if(m_Cursor+sizeof(uint64_t) <= m_Region->size()) {
      m_Buffer[m_Cursor]   = (uint8_t)(pValue >> 56);
      m_Buffer[m_Cursor+1] = (uint8_t)(pValue >> 48);
      m_Buffer[m_Cursor+2] = (uint8_t)(pValue >> 40);
      m_Buffer[m_Cursor+3] = (uint8_t)(pValue >> 32);
      m_Buffer[m_Cursor+4] = (uint8_t)(pValue >> 24);
      m_Buffer[m_Cursor+5] = (uint8_t)(pValue >> 16);
      m_Buffer[m_Cursor+6] = (uint8_t)(pValue >> 8);
      m_Buffer[m_Cursor+7] = (uint8_t)pValue;
    } 
    else 
      llvm::report_fatal_error("Memory access is out of boundary!"); 
  }

private:
  MemoryRegion *m_Region;
  bool m_SameEndian;
  Address m_Buffer;
  unsigned int m_Cursor;
};

} // namespace of mcld

#endif

