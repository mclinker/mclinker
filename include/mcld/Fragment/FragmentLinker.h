//===- FragmentLinker.h ---------------------------------------------------===//
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
#ifndef MCLD_FRAGMENT_FRAGMENT_LINKER_H
#define MCLD_FRAGMENT_FRAGMENT_LINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <set>
#include <string>

#include <llvm/ADT/ilist.h>

#include <mcld/LD/StaticResolver.h>
#include <mcld/LD/LDFileFormat.h>
#include <mcld/LD/LDContext.h>
#include <mcld/LD/Relocation.h>
#include <mcld/LD/SectionMerger.h>
#include <mcld/LD/Layout.h>
#include <mcld/MC/MCLDInput.h>
#include <mcld/Support/GCFactory.h>
#include <mcld/Support/GCFactoryListTraits.h>

namespace mcld {

class Module;
class TargetLDBackend;
class LinkerConfig;
class LDSection;
class LDSectionFactory;
class SectionData;
class SectionMap;
class Output;
class EhFrame;
class EhFrameHdr;
class MemoryArea;

/** \class FragmentLinker
 *  \brief FragmentLinker provides a pass to link object files.
 */
class FragmentLinker
{
public:
  enum DefinePolicy
  {
    Force,
    AsRefered
  };

  enum ResolvePolicy
  {
    Unresolve,
    Resolve
  };

public:
  FragmentLinker(const LinkerConfig& pConfig,
                 TargetLDBackend& pBackend,
                 Module& pModule,
                 SectionMap& pSectionMap);

  ~FragmentLinker();

  // ----- about symbols  ----- //
  /// addDynSymbol - add a symbol and resolve it immediately
  template<Input::Type FROM>
  LDSymbol* addSymbol(const llvm::StringRef& pName,
                      ResolveInfo::Type pType,
                      ResolveInfo::Desc pDesc,
                      ResolveInfo::Binding pBinding,
                      ResolveInfo::SizeType pSize,
                      LDSymbol::ValueType pValue,
                      FragmentRef* pFragmentRef,
                      ResolveInfo::Visibility pVisibility = ResolveInfo::Default);

  /// defineSymbol - add a symbol
  /// defineSymbol define a output symbol
  ///
  /// @tparam POLICY idicate how to define the symbol.
  ///   - Force
  ///     - Define the symbol forcefully. If the symbol has existed, override
  ///       it. Otherwise, define it.
  ///   - AsRefered
  ///     - If the symbol has existed, override it. Otherwise, return NULL
  ///       immediately.
  ///
  /// @tparam RESOLVE indicate whether to resolve the symbol or not.
  ///   - Unresolve
  ///      - Do not resolve the symbol, and override the symbol immediately.
  ///   - Resolve
  ///      - Resolve the defined symbol.
  ///
  /// @return If the output symbol has existed, return it. Otherwise, create
  ///         a new symbol and return the new one.
  template<DefinePolicy POLICY, ResolvePolicy RESOLVE>
  LDSymbol* defineSymbol(const llvm::StringRef& pName,
                         bool pIsDyn,
                         ResolveInfo::Type pType,
                         ResolveInfo::Desc pDesc,
                         ResolveInfo::Binding pBinding,
                         ResolveInfo::SizeType pSize,
                         LDSymbol::ValueType pValue,
                         FragmentRef* pFragmentRef,
                         ResolveInfo::Visibility pVisibility = ResolveInfo::Default);

  bool finalizeSymbols();

  // -----  sections  ----- //
  /// getSectionMap - getSectionMap to change the behavior of SectionMerger
  /// SectionMap& getSectionMap()
  /// { return m_SectionMap; }

  /// createSectHdr - for reader and standard/target format to create a section
  /// header. This function will create a new LDSection and return it. If the
  /// output has no related LDSection, this function will also create one and
  /// push into the output.
  LDSection& createSectHdr(const std::string& pName,
                           LDFileFormat::Kind pKind,
                           uint32_t pType,
                           uint32_t pFlag);

  /// getOrCreateOutputSectHdr - for reader and standard/target format to get
  /// or create the output's section header
  LDSection& getOrCreateOutputSectHdr(const std::string& pName,
                                      LDFileFormat::Kind pKind,
                                      uint32_t pType,
                                      uint32_t pFlag,
                                      uint32_t pAlign = 0x0);

  /// getOrCreateSectData - for reader to map and perform section merging immediately
  SectionData& getOrCreateSectData(LDSection& pSection);

  // -----  relocations  ----- //
  /// addRelocation - add a relocation entry in FragmentLinker (only for object file)
  /// @param pType - the type of the relocation
  /// @param pResolveInfo - the symbol should be the symbol in the input file. FragmentLinker
  ///                  computes the real applied address by the output symbol.
  /// @param pFragmentRef - the fragment reference of the applied address.
  /// @param pTargetSection - the section of the relocation applying target
  /// @param pAddend - the addend value for applying relocation
  Relocation* addRelocation(Relocation::Type pType,
                            const LDSymbol& pSym,
                            ResolveInfo& pResolveInfo,
                            FragmentRef& pFragmentRef,
                            const LDSection& pTargetSection,
                            Relocation::Address pAddend = 0);

  /// applyRelocations - apply all relocation enties.
  bool applyRelocations();

  /// syncRelocationResult - After applying relocation, write back relocation target
  /// data to output file.
  void syncRelocationResult(MemoryArea& pOutput);

  // -----  layout  ----- //
  void initSectionMap();

  Layout& getLayout()
  { return m_Layout; }

  const Layout& getLayout() const
  { return m_Layout; }

  bool layout();

  // -----  capacity  ----- //
  const LinkerConfig& getLDInfo() const { return m_Config; }

  // ----- output attribute ----- //
  /// isOutputPIC - return whether the output is position-independent
  bool isOutputPIC() const;

  /// isStaticLink - return whether we're doing static link
  bool isStaticLink() const;

 private:
  LDSymbol* defineSymbolForcefully(const llvm::StringRef& pName,
                                   bool pIsDyn,
                                   ResolveInfo::Type pType,
                                   ResolveInfo::Desc pDesc,
                                   ResolveInfo::Binding pBinding,
                                   ResolveInfo::SizeType pSize,
                                   LDSymbol::ValueType pValue,
                                   FragmentRef* pFragmentRef,
                                   ResolveInfo::Visibility pVisibility);

  LDSymbol* defineAndResolveSymbolForcefully(const llvm::StringRef& pName,
                                             bool pIsDyn,
                                             ResolveInfo::Type pType,
                                             ResolveInfo::Desc pDesc,
                                             ResolveInfo::Binding pBinding,
                                             ResolveInfo::SizeType pSize,
                                             LDSymbol::ValueType pValue,
                                             FragmentRef* pFragmentRef,
                                             ResolveInfo::Visibility pVisibility);

  LDSymbol* defineSymbolAsRefered(const llvm::StringRef& pName,
                                  bool pIsDyn,
                                  ResolveInfo::Type pType,
                                  ResolveInfo::Desc pDesc,
                                  ResolveInfo::Binding pBinding,
                                  ResolveInfo::SizeType pSize,
                                  LDSymbol::ValueType pValue,
                                  FragmentRef* pFragmentRef,
                                  ResolveInfo::Visibility pVisibility);

  LDSymbol* defineAndResolveSymbolAsRefered(const llvm::StringRef& pName,
                                            bool pIsDyn,
                                            ResolveInfo::Type pType,
                                            ResolveInfo::Desc pDesc,
                                            ResolveInfo::Binding pBinding,
                                            ResolveInfo::SizeType pSize,
                                            LDSymbol::ValueType pValue,
                                            FragmentRef* pFragmentRef,
                                            ResolveInfo::Visibility pVisibility);

  bool shouldForceLocal(const ResolveInfo& pInfo) const;

  LDSymbol* addSymbolFromDynObj(const llvm::StringRef& pName,
                                ResolveInfo::Type pType,
                                ResolveInfo::Desc pDesc,
                                ResolveInfo::Binding pBinding,
                                ResolveInfo::SizeType pSize,
                                LDSymbol::ValueType pValue,
                                FragmentRef* pFragmentRef,
                                ResolveInfo::Visibility pVisibility);

  LDSymbol* addSymbolFromObject(const llvm::StringRef& pName,
                                ResolveInfo::Type pType,
                                ResolveInfo::Desc pDesc,
                                ResolveInfo::Binding pBinding,
                                ResolveInfo::SizeType pSize,
                                LDSymbol::ValueType pValue,
                                FragmentRef* pFragmentRef,
                                ResolveInfo::Visibility pVisibility);

  /// checkIsOutputPIC - return whether the output is position-independent,
  /// called by isOutputPIC()
  bool checkIsOutputPIC() const;

  /// checkIsStaticLink - return whether we're doing static link, called by
  /// isStaticLink()
  bool checkIsStaticLink() const;

private:
  typedef GCFactory<LDSymbol, 0> LDSymbolFactory;

private:
  const LinkerConfig& m_Config;
  TargetLDBackend& m_Backend;
  Module& m_Module;
  SectionMap& m_SectionMap;
  LDSymbolFactory m_LDSymbolFactory;
  SectionMerger* m_pSectionMerger;
  Layout m_Layout;
};

#include "FragmentLinker.tcc"

} // namespace of mcld

#endif

