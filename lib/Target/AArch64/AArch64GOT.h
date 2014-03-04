//===- AArch64GOT.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TARGET_AARCH64_AARCH64GOT_H
#define TARGET_AARCH64_AARCH64GOT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Support/MemoryRegion.h>
#include <mcld/Target/GOT.h>

#include <llvm/ADT/DenseMap.h>

#include <vector>

namespace mcld {

class LDSection;

/** \class AArch64GOTEntry
 *  \brief GOT Entry with size of 8 bytes
 */
class AArch64GOTEntry : public GOT::Entry<8>
{
public:
  AArch64GOTEntry(uint64_t pContent, SectionData* pParent)
   : GOT::Entry<8>(pContent, pParent)
  {}
};

/** \class AArch64GOT
 *  \brief AArch64 Global Offset Table.
 *
 *  AArch64 GOT integrates traditional .got.plt and .got sections into one.
 *  Traditional .got.plt is placed in the front part of GOT (PLTGOT), and
 *  traditional .got is placed in the rear part of GOT (GOT). When -z now and
 *  -z relro are given, the got section layout will be as below. Otherwise,
 *  there will be two seperated sections, .got and .got.plt.
 *
 *  AArch64 .got
 *            +--------------+
 *            |    GOT0      |
 *            +--------------+
 *            |    GOTPLT    |
 *            +--------------+
 *            |    GOT       |
 *            +--------------+
 *
 */
class AArch64GOT : public GOT
{
public:
  /// AArch64GOT - constructor of AArch64GOT
  /// @param pHasGOT0 - GOT0 is the default entries of GOTPLT sectiion. If this
  /// section contains only GOT entries (and does not have GOTPLT entries), then
  /// pHasGOT0 should be false.
  AArch64GOT(LDSection &pSection, bool pHasGOT0);

  ~AArch64GOT();

  AArch64GOTEntry* createGOT();
  AArch64GOTEntry* createGOTPLT();

  void finalizeSectionSize();

  uint64_t emit(MemoryRegion& pRegion);

  void applyGOT0(uint64_t pAddress);

  void applyGOTPLT(uint64_t pPLTBase);

  bool hasGOT1() const;

private:
  typedef std::vector<AArch64GOTEntry*> EntryListType;
  typedef EntryListType::iterator entry_iterator;
  typedef EntryListType::const_iterator const_entry_iterator;

private:
  AArch64GOTEntry* m_pGOTPLTFront;
  AArch64GOTEntry* m_pGOTFront;

  /// m_GOTPLTEntries - a list of gotplt entries
  EntryListType m_GOTPLT;

  /// m_GOTEntris - a list of got entries
  EntryListType m_GOT;
};

} // namespace of mcld

#endif

