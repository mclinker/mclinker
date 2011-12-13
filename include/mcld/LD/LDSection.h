//===- LDSection.h --------------------------------------------------------===//
//
//                     The MCLinker Project
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

#include "llvm/MC/MCSection.h"
#include "llvm/MC/SectionKind.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/DataTypes.h"

namespace mcld {

/** \class LDSection
 *  \brief LDSection represents a section header entry. It is a unified
 *  abstraction for various file formats.
 *
 *  LDSection contains both the format-dependent data and LLVM specific data.
 *  
 */
class LDSection : public llvm::MCSection
{
public:
  LDSection(llvm::SectionKind pKind,
            const llvm::StringRef& pName,
            uint64_t pSize,
            uint64_t pOffset,
            uint64_t pAddr,
            uint32_t pFlag,
            uint32_t pType);

  const llvm::StringRef& name() const
  { return m_Name; }

  /// size - An integer specifying the size in bytes of the virtual memory
  /// occupied by this section.
  ///   In ELF, if the type() is SHT_NOBITS, this function return zero.
  ///   Before layouting, output's LDSection::size() should return zero.
  uint64_t size() const
  { return m_Size; }

  /// offset - An integer specifying the offset of this section in the file.
  ///   Before layouting, output's LDSection::offset() should return zero.
  uint64_t offset() const
  { return m_Offset; }

  /// addr - An integer specifying the offset of this section in the file.
  ///   Before layouting, output's LDSection::offset() should return zero.
  ///   ELF uses sh_addralign to set alignment constraints. In LLVM, alignment
  ///   constraint is set in MCSectionData::setAlignment. addr() contains the
  ///   original ELF::sh_addr. Modulo sh_addr by sh_addralign is not necessary.
  ///   MachO uses the same scenario.
  ///
  ///   Because addr() in output is changing during linking, MCLinker does not
  ///   store the address of the output here. The address is in Layout
  uint64_t addr() const
  { return m_Addr; }

  /// flag - An integer describes miscellaneous attributes.
  ///   In ELF, it is sh_flags.
  ///   In MachO, it's attribute field of struct section::flags
  uint32_t flag() const
  { return m_Flag; }

  /// type - The categorizes the section's contents and semantics. It's
  /// different from llvm::SectionKind. Type is format-dependent, but
  /// llvm::SectionKind is format independent and is used for bit-code.
  ///   In ELF, it is sh_type
  ///   In MachO, it's type field of struct section::flags
  uint32_t type() const
  { return m_Type; }

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

  static bool classof(const MCSection *S)
  { return S->getVariant() == SV_LDContext; }

  static bool classof(const LDSection *)
  { return true; }

private:
  llvm::StringRef m_Name;

  uint64_t m_Size;
  uint64_t m_Offset;
  uint64_t m_Addr;

  uint32_t m_Flag;
  uint32_t m_Type;

}; // end of LDSection

} // end namespace mcld

#endif

