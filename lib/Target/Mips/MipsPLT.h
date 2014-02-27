//===- MipsPLT.h ----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TARGET_MIPS_MIPSPLT_H
#define TARGET_MIPS_MIPSPLT_H

#include <mcld/Target/PLT.h>
#include <mcld/Support/MemoryRegion.h>

namespace mcld {

class MipsGOTPLT;

//===----------------------------------------------------------------------===//
// MipsPLT
//===----------------------------------------------------------------------===//
/** \class MipsPLT
 *  \brief Mips Procedure Linkage Table
 */
class MipsPLT : public PLT
{
public:
  MipsPLT(LDSection& pSection);

  // hasPLT1 - return if this PLT has any PLTA/PLTB entries
  bool hasPLT1() const;

  uint64_t emit(MemoryRegion& pRegion);

  Fragment* consume();

  void applyAllPLT(MipsGOTPLT& pGOTPLT);

public:
  // PLT
  void reserveEntry(size_t pNum = 1);
  void finalizeSectionSize();

private:
  // the last consumed entry.
  SectionData::iterator m_Last;
};

} // namespace of mcld

#endif
