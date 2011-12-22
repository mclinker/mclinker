//===- ARMPLT.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ARM_PLT_H
#define ARM_PLT_H

#include "mcld/Target/PLT.h"

namespace mcld {

class ARMGOT;

static const uint32_t arm_plt0[] = {
  0xe52de004, // str   lr, [sp, #-4]!
  0xe59fe004, // ldr   lr, [pc, #4]
  0xe08fe00e, // add   lr, pc, lr
  0xe5bef008, // ldr   pc, [lr, #8]!
  0x00000000, // &GOT[0] - .
};

static const uint32_t arm_plt1[] = {
  0xe28fc600, // add   ip, pc, #0xNN00000
  0xe28cca00, // add   ip, ip, #0xNN000
  0xe5bcf000, // ldr   pc, [ip, #0xNNN]!
};

class ARMPLT0 : public PLTEntry {
public:
  ARMPLT0(): PLTEntry(sizeof(arm_plt0), 0) {
    initPLTEntry();
  }

  ~ARMPLT0() {
    delete m_pContent;
    m_pContent = 0;
  }

  void initPLTEntry() {
    m_pContent = static_cast<unsigned char*>(malloc(m_EntrySize));

    if (!m_pContent)
      memcpy(m_pContent, arm_plt0, sizeof(arm_plt0));
  }
};

class ARMPLT1 : public PLTEntry {
public:
  ARMPLT1() : PLTEntry(sizeof(arm_plt1),0) {
    initPLTEntry();
  };

  ~ARMPLT1() {
    delete m_pContent;
    m_pContent = 0;
  }

  void initPLTEntry() {
    m_pContent = static_cast<unsigned char*>(malloc(m_EntrySize));

    if (!m_pContent)
      memcpy(m_pContent, arm_plt1, sizeof(arm_plt1));
  }
};

/** \class ARMPLT
 *  \brief ARM Procedure Linkage Table
 */
class ARMPLT : public PLT
{
  typedef llvm::DenseMap<const ResolveInfo*, ARMPLT1*> SymbolIndexType;

public:
  ARMPLT(const LDSection& pSection, ARMGOT& pGOT);
  ~ARMPLT();

public:

  // Override virtual function.
  // Additionally, reserveEntry is ARMGOT friend function.
  void reserveEntry(int pNum = 1) ;

  PLTEntry* getEntry(const ResolveInfo& pSymbol, bool& pExist) ;

private:
  ARMGOT& m_GOT;
  SymbolIndexType m_SymbolIndexMap;
};

} // namespace of mcld

#endif

