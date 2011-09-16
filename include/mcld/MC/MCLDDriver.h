/*****************************************************************************
 *   The mcld Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@mediatek.com>                                         *
 *   Luba Tang <luba.tang@mediatek.com>                                      *
 ****************************************************************************/
#ifndef MCLDDRIVER_H
#define MCLDDRIVER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/ADT/StringRef.h>
#include <mcld/MC/MCLDInfo.h>

namespace mcld {
class MCLDInfo;
class TargetLDBackend;

//===----------------------------------------------------------------------===//
/// MCLDDriver - MCLDDriver prepares parameters for MCLinker.
///
class MCLDDriver
{
public:
  MCLDDriver(MCLDInfo& pLDInfo, TargetLDBackend& pLDBackend);
  ~MCLDDriver();

  void normalize();

  /// linkable - check the linkability of current MCLDInfo
  //  Check list:
  //  - check the Attributes are not violate the constaint
  //  - check every Input has a correct Attribute
  bool linkable() const;

  /// readSymbolTables - read symbol tables from the input files.
  //  for each input file, loads its symbol table from file.
  void readSymbolTables();

  /// mergeSymbolTables - merge the symbol tables of input files into the
  /// output's symbol table.
  void mergeSymbolTables();

  /// relocation - applying relocation entries and create relocation
  /// section in the output files
  // Create relocation section, asking TargetLDBackend to
  // read the relocation information into RelocationEntry
  // and push_back into the relocation section
  void relocation();

private:
  TargetLDBackend &m_LDBackend;
  MCLDInfo& m_LDInfo;
};

} //end namespace mcld

#endif

