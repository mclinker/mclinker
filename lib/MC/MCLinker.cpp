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
                   SectionMap& pSectionMap,
                   const Resolver& pResolver)
: m_Backend(pBackend),
  m_Info(pInfo),
  m_Output(pContext),
  m_StrSymPool(pResolver),
  m_SectionMap(pSectionMap),
  m_LDSymbolFactory(128),
  m_LDSectHdrFactory(10) // the average number of sections. (assuming 10.)
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
                                    ResolveInfo::SizeType pSize,
                                    ResolveInfo::Visibility pVisibility)
{
  if (pBinding == ResolveInfo::Local)
    return NULL;

  // <resolved_info, exist?>
  std::pair<ResolveInfo*, bool> resolved_info = m_StrSymPool.insertSymbol(pName,
                                                                          pIsDyn,
                                                                          pDesc,
                                                                          pBinding,
                                                                          pSize,
                                                                          pVisibility);

  // the return ResolveInfo should not NULL
  assert(NULL != resolved_info.first);

  // create a LDSymbol for the input file.
  LDSymbol* input_sym = m_LDSymbolFactory.allocate();
  new (input_sym) LDSymbol();
  input_sym->setResolveInfo(*resolved_info.first);

  // if it is a new symbol, create a LDSymbol for the output
  if (!resolved_info.second) {
    LDSymbol* output_sym = m_LDSymbolFactory.allocate();
    new (output_sym) LDSymbol();
    output_sym->setResolveInfo(*resolved_info.first);
    m_Output.symtab().push_back(output_sym);
  }
  return input_sym;
}

LDSymbol* MCLinker::addLocalSymbol(const llvm::StringRef& pName,
                                   ResolveInfo::Desc pDesc,
                                   ResolveInfo::SizeType pSize,
                                   ResolveInfo::Visibility pVisibility)
{
  ResolveInfo* resolved_info =  m_StrSymPool.createSymbol(pName,
                                                          false,
                                                          pDesc,
                                                          ResolveInfo::Local,
                                                          pSize,
                                                          pVisibility);

  // the return ResolveInfo should not NULL
  assert(NULL != resolved_info);

  // create a LDSymbol for the input file.
  LDSymbol* input_sym = m_LDSymbolFactory.allocate();
  new (input_sym) LDSymbol();
  input_sym->setResolveInfo(*resolved_info);

  // create a LDSymbol for the output
  LDSymbol* output_sym = m_LDSymbolFactory.allocate();
  new (output_sym) LDSymbol();
  output_sym->setResolveInfo(*resolved_info);
  m_Output.symtab().push_back(output_sym);

  return input_sym;
}

LDSection* MCLinker::createSectHdr(const std::string& pName,
                                   LDFileFormat::Kind pKind,
                                   uint32_t pType,
                                   uint32_t pFlag)
{
  // for user such as reader, standard/target fromat
  LDSection* result =
    m_LDSectHdrFactory.produce(pName, pKind, pType, pFlag);

  // check if we need to create a output section for output LDContext
  const std::string& sect_name = m_SectionMap.getOutputSectName(pName);
  LDSection* output_sect = m_Output.getSection(sect_name);

  if (NULL == output_sect) {
  // create a output section and push it into output LDContext
    output_sect =
      m_LDSectHdrFactory.produce(sect_name, pKind, pType, pFlag);
    m_Output.getSectionTable().push_back(output_sect);
  }
  return result;
}

llvm::MCSectionData* MCLinker::getOrCreateSectData(LDSection* pSection)
{
  return NULL;
}
