//===- OutputRelocSection.h -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_OUTPUT_RELOCATION_SECTION_H
#define MCLD_OUTPUT_RELOCATION_SECTION_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/LD/RelocData.h>

namespace mcld
{

class LDSymbol;
class Module;
class Relocation;
class RelocationFactory;

/** \class OutputRelocSection
 *  \brief Dynamic relocation section for ARM .rel.dyn and .rel.plt
 */
class OutputRelocSection
{
public:
  OutputRelocSection(Module& pModule,
                     LDSection& pSection,
                     unsigned int pEntrySize);

  ~OutputRelocSection();

  void reserveEntry(RelocationFactory& pRelFactory, size_t pNum=1);

  Relocation* consumeEntry();

  void finalizeSectionSize();

  /// addSymbolToDynSym - add local symbol to TLS category so that it'll be
  /// emitted into .dynsym
  bool addSymbolToDynSym(LDSymbol& pSymbol);

  // ----- observers ----- //
  bool empty()
  { return m_pRelocData->empty(); }

private:
  typedef RelocData::iterator FragmentIterator;

private:
  Module& m_Module;

  /// m_pSection - LDSection of this Section
  LDSection* m_pSection;

  /// m_RelocData - the output RelocData which contains the dynamic
  /// relocations
  RelocData* m_pRelocData;

  /// m_EntryBytes - size of a relocation entry
  unsigned int m_EntryBytes;

  /// m_isVisit - First time visit the function getEntry() or not
  bool m_isVisit;

  /// m_ValidEntryIterator - point to the first valid entry
  FragmentIterator m_ValidEntryIterator;
};

} // namespace of mcld

#endif

