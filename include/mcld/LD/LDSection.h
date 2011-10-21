//===- LDSection.h --------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_LD_LDSECTION_H
#define MCLD_LD_LDSECTION_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/ADT/Uncopyable.h"

namespace mcld {

//LDSection represents a section header entry
//it is a unified abstraction for various file formats
class LDSection : public Uncopyable {
public:
  LDSection(char* Name,uint64_t Size,uint64_t Offset,
            uint64_t Addr, uint32_t Flag, uint32_t Type)
  : m_pName(Name), m_Size(Size), m_Offset(Offset),
    m_Addr(Addr), m_Flag(Flag), m_Type(Type) { }

  const char* name() const
  { return m_pName; }

  const uint64_t size() const
  { return m_Size; }

  const uint64_t offset() const
  { return m_Offset; }

  const uint64_t addr() const
  { return m_Addr; }

  const uint32_t flag() const
  { return m_Flag; }

  const uint32_t type() const
  { return m_Type; }

  void setName(const char* name)
  { m_pName = name; }

  void setSize(uint64_t size)
  { m_Size = size; }

  void setOffset(uint64_t Offset)
  { m_Offset = Offset; }

  void setAddr(uint64_t addr)
  { m_Addr = addr; }

  void setFlag(uint32_t flag)
  { m_Flag = flag; }

  void setType(uint32_t type)
  { m_Type = type; }

private:
  const char* m_pName;

  uint64_t m_Size;
  uint64_t m_Offset;
  uint64_t m_Addr;

  uint32_t m_Flag;
  uint32_t m_Type;

}; // end of LDSection

} // end namespace mcld
#endif

