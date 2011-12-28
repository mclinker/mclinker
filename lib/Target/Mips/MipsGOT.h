//===- MipsGOT.h ----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_MIPS_GOT_H
#define MCLD_MIPS_GOT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Target/GOT.h>

namespace mcld
{

class LDSection;

/** \class MipsGOT
 *  \brief Mips Global Offset Table.
 */
class MipsGOT : public GOT
{
public:
  typedef GOTEntry Entry;

public:
  // for target dependent section, target should maintain its Section size
  // in LDSection by target itself, so we let GOT holds a pointer to its
  // LDSection
  MipsGOT(LDSection& pSection, llvm::MCSectionData& pSectionData);
  ~MipsGOT();

  // While reserveEntry, the size in LDSection is also updated. As the result,
  // layout will get the correct size of GOT section
  void reserveEntry(int pNum = 1);

  Entry* getEntry(const ResolveInfo& pSymbol, bool& pExist);
};

} // namespace of mcld

#endif

