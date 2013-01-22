//===- FragmentLinker.tcc --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// defineSymbol - define a new symbol
template<FragmentLinker::DefinePolicy POLICY,
         FragmentLinker::ResolvePolicy RESOLVE>
LDSymbol* FragmentLinker::defineSymbol(const llvm::StringRef& pName,
                                       ResolveInfo::Type pType,
                                       ResolveInfo::Desc pDesc,
                                       ResolveInfo::Binding pBinding,
                                       ResolveInfo::SizeType pSize,
                                       LDSymbol::ValueType pValue,
                                       FragmentRef* pFragmentRef,
                                       ResolveInfo::Visibility pVisibility)
{
  // These if/return should be optimized by compiler.
  // This function is defined for clarity.
  if (FragmentLinker::Force == POLICY && FragmentLinker::Unresolve == RESOLVE)
    return defineSymbolForcefully(pName,
                                  pType,
                                  pDesc,
                                  pBinding,
                                  pSize,
                                  pValue,
                                  pFragmentRef,
                                  pVisibility);

  if (FragmentLinker::AsRefered == POLICY && FragmentLinker::Unresolve == RESOLVE)
    return defineSymbolAsRefered(pName,
                                 pType,
                                 pDesc,
                                 pBinding,
                                 pSize,
                                 pValue,
                                 pFragmentRef,
                                 pVisibility);

  if (FragmentLinker::Force == POLICY && FragmentLinker::Resolve == RESOLVE)
    return defineAndResolveSymbolForcefully(pName,
                                            pType,
                                            pDesc,
                                            pBinding,
                                            pSize,
                                            pValue,
                                            pFragmentRef,
                                            pVisibility);

  if (FragmentLinker::AsRefered == POLICY && FragmentLinker::Resolve == RESOLVE)
    return defineAndResolveSymbolAsRefered(pName,
                                           pType,
                                           pDesc,
                                           pBinding,
                                           pSize,
                                           pValue,
                                           pFragmentRef,
                                           pVisibility);
}

