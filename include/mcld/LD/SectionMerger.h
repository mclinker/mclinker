//===- header.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef SECTIONMERGER_H
#define SECTIONMERGER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <map>
#include <string>
#include <llvm/MC/MCAssembler.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/LD/SectionMap.h>

namespace mcld
{
class MCLinker;
class SectionMap;

/** \class SectionMerger
 *  \brief maintain the mappings of section name to associated section data
 */
class SectionMerger
{
public:
  typedef std::map<const std::string, llvm::MCSectionData*> SectionDataMapTy;

  typedef SectionDataMapTy::iterator iterator;
  typedef SectionDataMapTy::const_iterator const_iterator;

public:
  SectionMerger(MCLinker& pLinker);
  ~SectionMerger();

  llvm::MCSectionData& getOutputSectionData(const std::string& pInputSectName);

  // -----  iterators  ----- //
  iterator begin()
  { return m_SectionDataMap.begin(); }

  iterator end()
  { return m_SectionDataMap.end(); }

  const_iterator begin() const
  { return m_SectionDataMap.begin(); }

  const_iterator end() const
  { return m_SectionDataMap.end(); }

private:
  MCLinker& m_Linker;
  MCLinker::SectionListTy& m_SectionList;
  SectionMap& m_SectionMap;
  SectionDataMapTy m_SectionDataMap;
};

} // namespace of mcld

#endif

