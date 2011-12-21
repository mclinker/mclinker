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
#include <string>
#include <llvm/MC/MCAssembler.h>
#include <mcld/LD/StrSymPool.h>
#include <mcld/LD/StaticResolver.h>
#include <mcld/LD/LDSectionFactory.h>
#include <mcld/LD/LDFileFormat.h>
#include <mcld/LD/LDContext.h>
#include <mcld/Support/GCFactory.h>

namespace mcld {

class Input;
class Layout;
class TargetLDBackend;
class MCLDInfo;
class LDSection;
class LDSectionFactory;
class SectionMap;

/** \class MCLinker
 *  \brief MCLinker provides a pass to link object files.
 */
class MCLinker
{
public:
  MCLinker(TargetLDBackend& pBackend,
           MCLDInfo& pLDInfo,
           LDContext& pContext,
           SectionMap& pSectionMap,
           const Resolver& pResolver = StaticResolver());
  ~MCLinker();

  // ----- about symbols  ----- //
  /// addGlobalSymbol - add a symbol and resolve it immediately
  LDSymbol* addGlobalSymbol(const llvm::StringRef& pName,
                            bool pIsDyn,
                            ResolveInfo::Desc pDesc,
                            ResolveInfo::Binding pBinding,
                            ResolveInfo::SizeType pSize,
                            ResolveInfo::Visibility pVisibility = ResolveInfo::Default);

  /// addLocalSymbol - create a local symbol and add it into the output.
  LDSymbol* addLocalSymbol(const llvm::StringRef& pName,
                           ResolveInfo::Desc pDesc,
                           ResolveInfo::SizeType pSize,
                           ResolveInfo::Visibility pVisibility = ResolveInfo::Default);

  /// mergeSymbolTable - merge the symbol table and resolve symbols.
  ///   Since in current design, MCLinker resolves symbols when reading symbol
  ///   tables, this function do nothing.
  bool mergeSymbolTable(Input& pInput)
  { return true; }

  // -----  sections  ----- //
  /// getSectionMap - getSectionMap to change the behavior of SectionMerger
  /// SectionMap& getSectionMap()
  /// { return m_SectionMap; }

  /// getOrCreateSectHdr - for standard/target format to get or create a
  ///                      section header of output file
  LDSection* getOrCreateSectHdr(const std::string& pName,
                                LDFileFormat::Kind pKind,
                                uint32_t pType,
                                uint32_t pFlag);

  /// getOrCreateSectData - for reader to map and perform section merging immediately
  llvm::MCSectionData* getOrCreateSectData(const std::string& pName);

  // -----  capacity  ----- //
  MCLDInfo& getLDInfo()
  { return m_Info; }

  const MCLDInfo& getLDInfo() const
  { return m_Info; }

private:
  typedef GCFactory<LDSymbol, 0> LDSymbolFactory;

private:
  TargetLDBackend& m_Backend;
  MCLDInfo& m_Info;
  LDContext& m_Output;
  StrSymPool m_StrSymPool;
  LDSymbolFactory m_LDSymbolFactory;
  LDSectionFactory m_OutputSectHdrFactory;
  SectionMap& m_SectionMap;
};

} // namespace of mcld

#endif

