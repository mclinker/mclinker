//===- MCLinker.tcc -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

/// addSymbol - add a symbol and resolve it immediately
template<Input::Type FROM>
LDSymbol* MCLinker::addSymbol(const llvm::StringRef& pName,
                              ResolveInfo::Type pType,
                              ResolveInfo::Desc pDesc,
                              ResolveInfo::Binding pBinding,
                              ResolveInfo::SizeType pSize,
                              LDSymbol::ValueType pValue,
                              MCFragmentRef* pFragmentRef,
                              ResolveInfo::Visibility pVisibility)
{
  llvm::StringRef symbol_name = pName;
  // --wrap and symbol script
  if (!getLDInfo().scripts().wrapMap().empty() &&
      ResolveInfo::Undefined == pDesc) {
    ScriptOptions::SymbolWrapMap::iterator wrapSym
                                 = getLDInfo().scripts().wrapMap().find(pName);
    if (wrapSym != getLDInfo().scripts().wrapMap().end()) {
      llvm::errs() << "turn " << pName << " into " << wrapSym.getEntry()->value() << "\n";
      symbol_name = wrapSym.getEntry()->value();
    }
  }

  if (FROM == Input::DynObj)
    return addSymbolFromDynObj(symbol_name,
                               pType,
                               pDesc,
                               pBinding,
                               pSize,
                               pValue,
                               pFragmentRef,
                               pVisibility);

  if (FROM == Input::Object)
    return addSymbolFromObject(symbol_name,
                               pType,
                               pDesc,
                               pBinding,
                               pSize,
                               pValue,
                               pFragmentRef,
                               pVisibility);

  llvm::report_fatal_error("add a symbol from unknown file type.\n");
  return NULL;
}

// defineSymbol - define a new symbol
template<MCLinker::DefinePolicy POLICY, MCLinker::ResolvePolicy RESOLVE>
LDSymbol* MCLinker::defineSymbol(const llvm::StringRef& pName,
                                 bool pIsDyn,
                                 ResolveInfo::Type pType,
                                 ResolveInfo::Desc pDesc,
                                 ResolveInfo::Binding pBinding,
                                 ResolveInfo::SizeType pSize,
                                 LDSymbol::ValueType pValue,
                                 MCFragmentRef* pFragmentRef,
                                 ResolveInfo::Visibility pVisibility)
{
  // These if/return should be optimized by compiler.
  // This function is defined for clarity.
  if (MCLinker::Force == POLICY && MCLinker::Unresolve == RESOLVE)
    return defineSymbolForcefully(pName,
                                  pIsDyn,
                                  pType,
                                  pDesc,
                                  pBinding,
                                  pSize,
                                  pValue,
                                  pFragmentRef,
                                  pVisibility);

  if (MCLinker::AsRefered == POLICY && MCLinker::Unresolve == RESOLVE)
    return defineSymbolAsRefered(pName,
                                 pIsDyn,
                                 pType,
                                 pDesc,
                                 pBinding,
                                 pSize,
                                 pValue,
                                 pFragmentRef,
                                 pVisibility);

  if (MCLinker::Force == POLICY && MCLinker::Resolve == RESOLVE)
    return defineAndResolveSymbolForcefully(pName,
                                            pIsDyn,
                                            pType,
                                            pDesc,
                                            pBinding,
                                            pSize,
                                            pValue,
                                            pFragmentRef,
                                            pVisibility);

  if (MCLinker::AsRefered == POLICY && MCLinker::Resolve == RESOLVE)
    return defineAndResolveSymbolAsRefered(pName,
                                           pIsDyn,
                                           pType,
                                           pDesc,
                                           pBinding,
                                           pSize,
                                           pValue,
                                           pFragmentRef,
                                           pVisibility);
}

