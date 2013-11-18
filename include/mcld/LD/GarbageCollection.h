//===- GarbageCollection.h ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LD_GARBAGECOLLECTION_H
#define MCLD_LD_GARBAGECOLLECTION_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <map>
#include <queue>
#include <set>
#include <vector>

namespace mcld {

class LDSection;
class LinkerConfig;
class Module;
class LDSymbol;

/** \class GarbageCollection
 *  \brief Implementation of garbage collection for --gc-section.
 *  @ref GNU gold, gc.
 */
class GarbageCollection
{
public:
  GarbageCollection(const LinkerConfig& pConfig, Module& pModule);
  ~GarbageCollection();

  /// setEntrySymbol - set up the entry symbol for executable
  void setEntrySymbol(const LDSymbol& pEntry)
  { m_pEntry = &pEntry; }

  /// run - do garbage collection
  bool run();

private:
  typedef std::set<const LDSection*> SectionListTy;
  typedef std::queue<const LDSection*> WorkListTy;
  typedef std::map<const LDSection*, SectionListTy> ReachedSectionsTy;
  typedef std::vector<const LDSection*> SectionVecTy;

private:
  void setUpReachedSections();
  void findReferencedSections();
  void stripSections();

  void getEntrySections(SectionVecTy& pEntry);

private:
  /// m_ReachedSections - map a section to the reachable sections list
  ReachedSectionsTy m_ReachedSections;

  /// m_ReferencedSections - a list of sections which can be reached from entry
  SectionListTy m_ReferencedSections;

  const LinkerConfig& m_Config;
  Module& m_Module;
  const LDSymbol* m_pEntry;
};

} // namespace of mcld

#endif

