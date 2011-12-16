//===- MCLinker.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides a number of APIs used by SectLinker.
// These APIs do the things which a linker should do.
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_MCLINKER_H
#define MCLD_MCLINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <vector>
#include <llvm/MC/MCAssembler.h>
#include <mcld/LD/StrSymPool.h>
#include <mcld/LD/StaticResolver.h>
#include <mcld/LD/SectionFactory.h>
#include <mcld/LD/LDFileFormat.h>
#include <mcld/LD/SectionMap.h>

namespace mcld {

class MCLDCommand;
class MCLDFile;
class Input;
class Layout;

class TargetLDBackend;
class MCLDInfo;

class SectionMap;

/** \class MCLinker
 *  \brief MCLinker provides a pass to link object files.
 */
class MCLinker
{
public:
  typedef std::vector<llvm::MCSectionData*> SectionListTy;
  typedef SectionListTy::iterator section_list_iterator;
  typedef SectionListTy::const_iterator const_section_list_iterator;

public:
  MCLinker(TargetLDBackend& pBackend,
           MCLDInfo& pLDInfo,
           const Resolver& pResolver = StaticResolver());
  ~MCLinker();

  /// mergeSymbolTable - merge the symbol table and resolve symbols.
  ///   Since in current design, MCLinker resolves symbols when reading symbol
  ///   tables, this function do nothing.
  bool mergeSymbolTable(Input& pInput)
  { return true; }

  // -----  observers  ----- //

  MCLDInfo& getLDInfo()
  { return m_Info; }

  const MCLDInfo& getLDInfo() const
  { return m_Info; }

  SectionMap& getSectionMap()
  { return m_SectionMap; }

  const SectionMap& getSectionMap() const
  { return m_SectionMap; }

  SectionListTy& getSectionList()
  { return m_SectionList; }

  const SectionListTy& getSectionList() const
  { return m_SectionList; }

  // -----  iterators ----- //
  section_list_iterator section_list_begin()
  { return m_SectionList.begin(); }

  section_list_iterator section_list_end()
  { return m_SectionList.end(); }

  const_section_list_iterator section_list_begin() const
  { return m_SectionList.begin(); }

  const_section_list_iterator section_list_end() const
  { return m_SectionList.end(); }

private:
  TargetLDBackend& m_Backend;
  MCLDInfo& m_Info;
  StrSymPool m_StrSymPool;
  SectionFactory m_SectionFactory;
  SectionMap m_SectionMap;
  std::vector<llvm::MCSectionData*> m_SectionList;
};

} // namespace of mcld

#endif

