//===- ObjectLinker.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// ObjectLinker plays the same role as GNU collect2 to prepare all implicit
// parameters for FragmentLinker.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_OBJECT_OBJECT_LINKER_H
#define MCLD_OBJECT_OBJECT_LINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <stddef.h>

namespace mcld {

class Module;
class LinkerConfig;
class IRBuilder;
class FragmentLinker;
class TargetLDBackend;
class MemoryArea;
class MemoryAreaFactory;
class ObjectReader;
class DynObjReader;
class ArchiveReader;
class GroupReader;
class BinaryReader;
class ObjectWriter;
class DynObjWriter;
class ExecWriter;
class BinaryWriter;

/** \class ObjectLinker
 *  \brief ObjectLinker prepares parameters for FragmentLinker.
 */
class ObjectLinker
{
public:
  ObjectLinker(const LinkerConfig& pConfig,
               Module& pModule,
               IRBuilder& pBuilder,
               TargetLDBackend& pLDBackend);

  ~ObjectLinker();

  /// initFragmentLinker - initialize FragmentLinker
  ///  Connect all components in FragmentLinker
  bool initFragmentLinker();

  /// initStdSections - initialize standard sections of the output file.
  bool initStdSections();

  /// normalize - normalize the input files
  void normalize();

  /// linkable - check the linkability of current LinkerConfig
  ///  Check list:
  ///  - check the Attributes are not violate the constaint
  ///  - check every Input has a correct Attribute
  bool linkable() const;

  /// readRelocations - read all relocation entries
  bool readRelocations();

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

  /// scanRelocations - scan all relocation entries by output symbols.
  bool scanRelocations();

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
  bool emitOutput(MemoryArea& pOutput);

  /// postProcessing - do modificatiion after all processes
  bool postProcessing(MemoryArea& pOutput);

  /// getLinker - get internal FragmentLinker object
  const FragmentLinker* getLinker() const { return m_pLinker; }
  FragmentLinker*       getLinker()       { return m_pLinker; }

  /// hasInitLinker - has Linker been initialized?
  bool hasInitLinker() const
  { return (NULL != m_pLinker); }

  // -----  readers and writers  ----- //
  const ObjectReader*  getObjectReader () const { return m_pObjectReader;  }
  ObjectReader*        getObjectReader ()       { return m_pObjectReader;  }

  const DynObjReader*  getDynObjReader () const { return m_pDynObjReader;  }
  DynObjReader*        getDynObjReader ()       { return m_pDynObjReader;  }

  const ArchiveReader* getArchiveReader() const { return m_pArchiveReader; }
  ArchiveReader*       getArchiveReader()       { return m_pArchiveReader; }

  const GroupReader*   getGroupReader  () const { return m_pGroupReader;   }
  GroupReader*         getGroupReader  ()       { return m_pGroupReader;   }

  const BinaryReader*  getBinaryReader () const { return m_pBinaryReader;  }
  BinaryReader*        getBinaryReader ()       { return m_pBinaryReader;  }

  const ObjectWriter*  getObjectWriter () const { return m_pObjectWriter;  }
  ObjectWriter*        getObjectWriter ()       { return m_pObjectWriter;  }

  const DynObjWriter*  getDynObjWriter () const { return m_pDynObjWriter;  }
  DynObjWriter*        getDynObjWriter ()       { return m_pDynObjWriter;  }

  const ExecWriter*    getExecWriter   () const { return m_pExecWriter;    }
  ExecWriter*          getExecWriter   ()       { return m_pExecWriter;    }

  const BinaryWriter*  getBinaryWriter () const { return m_pBinaryWriter;  }
  BinaryWriter*        getBinaryWriter ()       { return m_pBinaryWriter;  }

private:
  const LinkerConfig& m_Config;
  Module& m_Module;

  IRBuilder& m_Builder; 

  FragmentLinker* m_pLinker;
  TargetLDBackend &m_LDBackend;

  // -----  readers and writers  ----- //
  ObjectReader*  m_pObjectReader;
  DynObjReader*  m_pDynObjReader;
  ArchiveReader* m_pArchiveReader;
  GroupReader*   m_pGroupReader;
  BinaryReader*  m_pBinaryReader;
  ObjectWriter*  m_pObjectWriter;
  DynObjWriter*  m_pDynObjWriter;
  ExecWriter*    m_pExecWriter;
  BinaryWriter*  m_pBinaryWriter;
};

} // end namespace mcld
#endif
