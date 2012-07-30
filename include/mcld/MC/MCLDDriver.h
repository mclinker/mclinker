//===- MCLDDriver.h --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// MCLDDriver plays the same role as GNU collect2 to prepare all implicit
// parameters for MCLinker.
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_LDDRIVER_H
#define MCLD_LDDRIVER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/LD/SectionMap.h>

namespace mcld
{

class MCLinker;
class MCLDInfo;
class TargetLDBackend;
class MemoryAreaFactory;

/** \class MCLDDriver
 *  \brief MCLDDriver prepares parameters for MCLinker.
 */
class MCLDDriver
{
public:
  MCLDDriver(MCLDInfo& pLDInfo,
             TargetLDBackend& pLDBackend,
             MemoryAreaFactory& pAreaFactory);

  ~MCLDDriver();

  /// initMCLinker - initialize MCLinker
  ///  Connect all components in MCLinker
  bool initMCLinker();

  /// initStdSections - initialize standard sections of the output file.
  bool initStdSections();

  /// normalize - normalize the input files
  void normalize();

  /// linkable - check the linkability of current MCLDInfo
  ///  Check list:
  ///  - check the Attributes are not violate the constaint
  ///  - check every Input has a correct Attribute
  bool linkable() const;

  /// mergeSections - put allinput sections into output sections
  bool mergeSections();

  /// addStandardSymbols - shared object and executable files need some
  /// standard symbols
  ///   @return if there are some input symbols with the same name to the
  ///   standard symbols, return false
  bool addStandardSymbols();

  /// addTargetSymbols - some targets, such as MIPS and ARM, need some
  /// target-dependent symbols
  ///   @return if there are some input symbols with the same name to the
  ///   target symbols, return false
  bool addTargetSymbols();

  /// readRelocations - read all relocation entries
  bool readRelocations();

  /// prelayout - help backend to do some modification before layout
  bool prelayout();

  /// layout - linearly layout all output sections and reserve some space
  /// for GOT/PLT
  ///   Because we do not support instruction relaxing in this early version,
  ///   if there is a branch can not jump to its target, we return false
  ///   directly
  bool layout();

  /// postlayout - help backend to do some modification after layout
  bool postlayout();

  /// relocate - applying relocation entries and create relocation
  /// section in the output files
  /// Create relocation section, asking TargetLDBackend to
  /// read the relocation information into RelocationEntry
  /// and push_back into the relocation section
  bool relocation();

  /// finalizeSymbolValue - finalize the symbol value
  bool finalizeSymbolValue();

  /// emitOutput - emit the output file.
  bool emitOutput();

  /// postProcessing - do modificatiion after all processes
  bool postProcessing();

  /// getLinker - get internal MCLinker object
  MCLinker* getLinker()
  { return m_pLinker; }

  /// getLinker - get internal MCLinker object
  const MCLinker* getLinker() const
  { return m_pLinker; }

  /// hasInitLinker - has Linker been initialized?
  bool hasInitLinker() const
  { return (NULL != m_pLinker); }

private:
  MCLDInfo& m_LDInfo;
  TargetLDBackend &m_LDBackend;
  MCLinker* m_pLinker;
  SectionMap m_SectionMap;
  MemoryAreaFactory &m_AreaFactory;
};

} // end namespace mcld
#endif
