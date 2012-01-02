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
  MipsGOT(LDSection& pSection, llvm::MCSectionData& pSectionData);
  ~MipsGOT();

  // While reserveEntry, the size in LDSection is also updated. As the result,
  // layout will get the correct size of GOT section
  void reserveEntry(int pNum = 1);

  GOTEntry* getEntry(const ResolveInfo& pSymbol, bool& pExist);
};

} // namespace of mcld

#endif

