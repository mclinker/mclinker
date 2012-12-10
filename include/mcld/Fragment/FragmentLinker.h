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

#include <string>

#include <mcld/LinkerConfig.h>
#include <mcld/LD/LDFileFormat.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/Fragment/Relocation.h>
#include <mcld/MC/MCLDInput.h>

namespace mcld {

class Module;
class TargetLDBackend;
class LinkerConfig;
class LDSection;
class LDSectionFactory;
class SectionData;
class Output;
class EhFrame;
class EhFrameHdr;
class MemoryArea;
class RelocData;

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
                 Module& pModule,
                 TargetLDBackend& pBackend);

  ~FragmentLinker();

  // ----- about symbols  ----- //
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

  /// applyRelocations - apply all relocation enties.
  bool applyRelocations();

  /// syncRelocationResult - After applying relocation, write back relocation target
  /// data to output file.
  void syncRelocationResult(MemoryArea& pOutput);

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

  /// checkIsOutputPIC - return whether the output is position-independent,
  /// called by isOutputPIC()
  bool checkIsOutputPIC() const;

  /// checkIsStaticLink - return whether we're doing static link, called by
  /// isStaticLink()
  bool checkIsStaticLink() const;

  /// normalSyncRelocationResult - sync relocation result when producing shared
  /// objects or executables
  void normalSyncRelocationResult(MemoryArea& pOutput);

  /// partialSyncRelocationResult - sync relocation result when doing partial
  /// link
  void partialSyncRelocationResult(MemoryArea& pOutput);

  /// writeRelocationResult - helper function of syncRelocationResult, write
  /// relocation target data to output
  void writeRelocationResult(Relocation& pReloc, uint8_t* pOutput);

private:
  const LinkerConfig& m_Config;
  Module& m_Module;
  TargetLDBackend& m_Backend;
};

#include "FragmentLinker.tcc"

} // namespace of mcld

#endif

