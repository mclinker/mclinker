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

#include <set>
#include <string>

#include <llvm/ADT/ilist.h>

#include <mcld/LD/StaticResolver.h>
#include <mcld/LD/LDSectionFactory.h>
#include <mcld/LD/LDFileFormat.h>
#include <mcld/LD/LDContext.h>
#include <mcld/LD/Relocation.h>
#include <mcld/LD/SectionMerger.h>
#include <mcld/LD/Layout.h>
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/SymbolCategory.h>
#include <mcld/Support/GCFactory.h>
#include <mcld/Support/GCFactoryListTraits.h>

namespace mcld {

class TargetLDBackend;
class MCLDInfo;
class LDSection;
class LDSectionFactory;
class SectionData;
class SectionMap;
class Output;
class EhFrame;
class EhFrameHdr;

/** \class MCLinker
 *  \brief MCLinker provides a pass to link object files.
 */
class MCLinker
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
  MCLinker(TargetLDBackend& pBackend,
           MCLDInfo& pLDInfo,
           SectionMap& pSectionMap);

  ~MCLinker();

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

  // -----  eh_frame sections  ----- //
  /// addEhFrame - add an exception handling section
  /// @param pInput - the Input contains this section
  /// @param pSection - the input section
  /// @param pArea - the memory area which pSection is within.
  uint64_t addEhFrame(const Input& pInput,
                      LDSection& pSection,
                      MemoryArea& pArea);

  // -----  relocations  ----- //
  /// addRelocation - add a relocation entry in MCLinker (only for object file)
  /// @param pType - the type of the relocation
  /// @param pResolveInfo - the symbol should be the symbol in the input file. MCLinker
  ///                  computes the real applied address by the output symbol.
  /// @param pFragmentRef - the fragment reference of the applied address.
  /// @param pAddend - the addend value for applying relocation
  Relocation* addRelocation(Relocation::Type pType,
                            const LDSymbol& pSym,
                            ResolveInfo& pResolveInfo,
                            FragmentRef& pFragmentRef,
                            const LDSection& pSection,
                            Relocation::Address pAddend = 0);

  /// applyRelocations - apply all relocation enties.
  bool applyRelocations();

  /// syncRelocationResult - After applying relocation, write back relocation target
  /// data to output file.
  void syncRelocationResult();

  // -----  layout  ----- //
  void initSectionMap();

  Layout& getLayout()
  { return m_Layout; }

  const Layout& getLayout() const
  { return m_Layout; }

  bool layout();

  // -----  output symbols  ----- //
  SymbolCategory& getOutputSymbols()
  { return m_OutputSymbols; }

  const SymbolCategory& getOutputSymbols() const
  { return m_OutputSymbols; }

  // -----  capacity  ----- //
  MCLDInfo& getLDInfo()
  { return m_LDInfo; }

  const MCLDInfo& getLDInfo() const
  { return m_LDInfo; }

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
private:
  typedef GCFactory<LDSymbol, 0> LDSymbolFactory;
  typedef GCFactory<SectionData, 0> LDSectionDataFactory;
  typedef llvm::iplist<Fragment,
                       GCFactoryListTraits<Fragment> > RelocationListType;
  typedef std::set<LDSymbol*> ForceLocalSymbolTable;
  typedef std::vector<LDSymbol*> OutputSymbolTable;

private:
  TargetLDBackend& m_Backend;
  MCLDInfo& m_LDInfo;
  SectionMap& m_SectionMap;
  LDSymbolFactory m_LDSymbolFactory;
  LDSectionFactory m_LDSectHdrFactory;
  LDSectionDataFactory m_LDSectDataFactory;
  SectionMerger* m_pSectionMerger;
  Layout m_Layout;
  RelocationListType m_RelocationList;
  SymbolCategory m_OutputSymbols;
};

#include "MCLinker.tcc"

} // namespace of mcld

#endif

