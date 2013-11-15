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

#include <set>
#include <queue>
#include <map>

namespace mcld {

class LDSection;
class LinkerConfig;
class Module;

/** \class GarbageCollection
 *  \brief Implementation of garbage collection for --gc-section
 */
class GarbageCollection
{
public:
  GarbageCollection(const LinkerConfig& pConfig, Module& pModule);
  ~GarbageCollection();

  bool run();

private:
  typedef std::set<LDSection*> SectionListTy;
  typedef std::queue<LDSection*> WorkListTy;
  typedef std::map<LDSection*, SectionListTy> ReachedSectionsTy;

private:
  /// m_WorkList - a list of sections waiting to be processed
  WorkListTy m_WorkList;

  /// m_ReachedSections - map a section to the reachable sections list
  ReachedSectionsTy m_ReachedSections;

  /// m_ReferencedSections - a list of sections which can be
  SectionListTy m_ReferencedSections;

  const LinkerConfig& m_Config;
  Module& m_Module;
};

} // namespace of mcld

#endif

