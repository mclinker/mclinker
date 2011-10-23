//===- LDWriter.h ---------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  LDWriter provides an interface used by MCLinker,
//  which writes the result of linking into a .so file or a executable.
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_LD_LDWRITER
#define MCLD_LD_LDWRITER

#include "mcld/Support/MemoryArea.h"
#include "mcld/Support/MemoryAreaFactory.h"
#include "mcld/Support/MemoryRegion.h"

#include "llvm/Support/DataTypes.h"
#include <cassert>

namespace mcld{

class LDWriter {
public:
  virtual void writeFile() = 0;

protected:
  LDWriter(MemoryArea *Area, bool _IsLittleEndian)
  : m_pArea(Area), IsLittleEndian(_IsLittleEndian) {}

  virtual void writeHeader() = 0;

protected:
  virtual ~LDWriter() {};

  class RegionWriter {
    typedef unsigned char* Address;

    public:
      RegionWriter(MemoryRegion *Region, bool _IsLittleEndian)
      : m_Region(Region), m_Buffer(m_Region->getBuffer()),
        IsLittleEndian(_IsLittleEndian), Cursor(0) { }

      ~RegionWriter(){
        m_Region->sync();
      }

      void Write8(uint8_t Value) {
        if(!m_Buffer)
          assert(0 && "Try to write null memory buffer!");

        if(Cursor < m_Region->size()) {
          m_Buffer[Cursor] = char(Value);
          ++Cursor;
        } else
          assert(0 && "Memory access is out of boundary!");
      }

      void WriteLE32(uint32_t Value) {
        WriteLE16(uint16_t(Value >> 0));
        WriteLE16(uint16_t(Value >> 16));
      }

      void WriteLE64(uint64_t Value) {
        WriteLE32(uint32_t(Value >> 0));
        WriteLE32(uint32_t(Value >> 32));
      }

      void WriteBE16(uint16_t Value) {
        Write8(uint8_t(Value >> 8));
        Write8(uint8_t(Value >> 0));
      }

      void WriteBE32(uint32_t Value) {
        WriteBE16(uint16_t(Value >> 16));
        WriteBE16(uint16_t(Value >> 0));
      }

      void WriteBE64(uint64_t Value) {
        WriteBE32(uint32_t(Value >> 32));
        WriteBE32(uint32_t(Value >> 0));
      }

      void WriteLE16(uint16_t Value) {
        if (IsLittleEndian)
          WriteLE16(Value);
        else
          WriteBE16(Value);
      }

      void Write32(uint32_t Value) {
        if (IsLittleEndian)
          WriteLE32(Value);
        else
          WriteBE32(Value);
      }

      void Write64(uint64_t Value) {
       if (IsLittleEndian)
          WriteLE64(Value);
        else
          WriteBE64(Value);
      }

    private:
      void *operator new(size_t size) {}

    private:
      MemoryRegion *m_Region;

      const Address m_Buffer;
      bool IsLittleEndian;
      unsigned int Cursor;
    };

protected:
  MemoryArea *m_pArea;
  bool IsLittleEndian;
};
} //end namespace


#endif
