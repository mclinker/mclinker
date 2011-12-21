//===- header.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_X86_GOT_H
#define MCLD_X86_GOT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Target/GOT.h>

namespace mcld
{

/** \class X86GOT
 *  \brief X86 Global Offset Table.
 */
class X86GOT : public GOT
{
public:
  typedef GOTEntry Entry;
public:
  X86GOT(const LDSection& pSection);
  ~X86GOT();

  void reserveEntry(int pNum = 1);

  Entry* getEntry(const ResolveInfo& pSymbol, bool& pExist);

};

} // namespace of mcld

#endif

