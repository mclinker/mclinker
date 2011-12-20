//===- MCLinker.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the MCLinker class
//
//===----------------------------------------------------------------------===//

#include <mcld/MC/MCLinker.h>
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/LD/LDContext.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/LDSectionFactory.h>
#include <mcld/LD/SectionMap.h>
#include <mcld/Target/TargetLDBackend.h>
#include <llvm/Support/raw_ostream.h>
#include <cstdio>

using namespace mcld;

MCLinker::MCLinker(TargetLDBackend& pBackend,
                   MCLDInfo& pInfo,
                   LDContext& pContext,
//                   SectionMap& pSectionMap,
                   const Resolver& pResolver)
: m_Backend(pBackend),
  m_Info(pInfo),
  m_Output(pContext),
  m_StrSymPool(pResolver),
//  m_SectionMap(pSectionMap),
  m_LDSymbolFactory(128),
  m_OutputSectHdrFactory(10) // the average number of sections. (assuming 10.)
{
}

MCLinker::~MCLinker()
{
}

/// addSymbol - add a symbol and resolve it immediately
LDSymbol* MCLinker::addGlobalSymbol(const llvm::StringRef& pName,
                                    bool pIsDyn,
                                    ResolveInfo::Desc pDesc,
                                    ResolveInfo::Binding pBinding,
                                    ResolveInfo::ValueType pValue,
                                    ResolveInfo::SizeType pSize,
                                    ResolveInfo::Visibility pVisibility)
{
  if (pBinding == ResolveInfo::Local)
    return NULL;

  std::pair<ResolveInfo*, bool> resolved_result;
  resolved_result = m_StrSymPool.insertSymbol(pName,
                                             pIsDyn,
                                             pDesc,
                                             pBinding,
                                             pValue,
                                             pSize,
                                             pVisibility);

  LDSymbol* result = m_LDSymbolFactory.allocate();
  new (result) LDSymbol();
  if (NULL == resolved_result.first)
    return NULL;

  result->setResolveInfo(*resolved_result.first);
  m_Output.symtab().push_back(result);
  return result;
}

LDSymbol* MCLinker::addLocalSymbol(const llvm::StringRef& pName,
                                   ResolveInfo::Desc pDesc,
                                   ResolveInfo::ValueType pValue,
                                   ResolveInfo::SizeType pSize,
                                   ResolveInfo::Visibility pVisibility)
{
  ResolveInfo* resolve_info =  m_StrSymPool.createSymbol(pName,
                                                         false,
                                                         pDesc,
                                                         ResolveInfo::Local,
                                                         pValue,
                                                         pSize,
                                                         pVisibility);
  LDSymbol* result = m_LDSymbolFactory.allocate();
  new (result) LDSymbol();
  result->setResolveInfo(*resolve_info);

  m_Output.symtab().push_back(result);
  return result;
}

LDSection* MCLinker::getOrCreateSectHdr(const std::string& pName,
                                        LDFileFormat::Kind pKind,
                                        uint32_t pType,
                                        uint32_t pFlag)
{
  LDSection* result = m_OutputSectHdrFactory.find(pName);
  if (NULL == result)
    result = m_OutputSectHdrFactory.produce(pName, pKind, pType, pFlag);
  return result;
}

llvm::MCSectionData* MCLinker::getOrCreateSectData(const std::string& pName)
{
  return NULL;
}
