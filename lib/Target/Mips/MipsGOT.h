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
  MipsGOT(llvm::MCSectionData* pSectionData);
  ~MipsGOT();

  void reserveEntry(int pNum = 1);

  Entry* getEntry(const ResolveInfo& pSymbol, bool& pExist);
};

} // namespace of mcld

#endif

