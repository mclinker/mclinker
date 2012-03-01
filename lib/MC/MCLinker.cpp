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
#include <mcld/LD/Resolver.h>
#include <mcld/LD/LDContext.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/LDSectionFactory.h>
#include <mcld/LD/SectionMap.h>
#include <mcld/LD/RelocationFactory.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Target/TargetLDBackend.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>

using namespace mcld;

/// Constructor
MCLinker::MCLinker(TargetLDBackend& pBackend,
                   MCLDInfo& pInfo,
                   LDContext& pContext,
                   SectionMap& pSectionMap,
                   const Resolver& pResolver)
: m_Backend(pBackend),
  m_Info(pInfo),
  m_Output(pContext),
  m_SectionMap(pSectionMap),
  m_LDSymbolFactory(128),
  m_LDSectHdrFactory(10), // the average number of sections. (assuming 10.)
  m_LDSectDataFactory(10),
  m_SectionMerger(pSectionMap, pContext),
  m_StrSymPool(pResolver, 128)
{
  m_Info.setNamePool(m_StrSymPool);
}

/// Destructor
MCLinker::~MCLinker()
{
}

/// addSymbolFromObject - add a symbol from object file and resolve it
/// immediately
LDSymbol* MCLinker::addSymbolFromObject(const llvm::StringRef& pName,
                                        ResolveInfo::Type pType,
                                        ResolveInfo::Desc pDesc,
                                        ResolveInfo::Binding pBinding,
                                        ResolveInfo::SizeType pSize,
                                        LDSymbol::ValueType pValue,
                                        MCFragmentRef* pFragmentRef,
                                        ResolveInfo::Visibility pVisibility)
{

  // resolved_result is a triple <resolved_info, existent, override>
  Resolver::Result resolved_result;
  ResolveInfo old_info; // used for arrange output symbols

  if (pBinding == ResolveInfo::Local) {
    // if the symbol is a local symbol, create a LDSymbol for input, but do not
    // resolve them.
    resolved_result.info     = m_StrSymPool.createSymbol(pName,
                                                         false,
                                                         pType,
                                                         pDesc,
                                                         pBinding,
                                                         pSize,
                                                         pVisibility);

    // No matter if there is a symbol with the same name, insert the symbol
    // into output symbol table. So, we let the existent false.
    resolved_result.existent  = false;
    resolved_result.overriden = true;
  }
  else {
    // if the symbol is not local, insert and resolve it immediately
    m_StrSymPool.insertSymbol(pName, false, pType, pDesc, pBinding, pSize,
                              pVisibility, &old_info, resolved_result);
  }

  // the return ResolveInfo should not NULL
  assert(NULL != resolved_result.info);

  // create a LDSymbol for the input file.
  LDSymbol* input_sym = m_LDSymbolFactory.allocate();
  new (input_sym) LDSymbol();

  // set the relation between input LDSymbol and its ResolveInfo
  input_sym->setResolveInfo(*resolved_result.info);

  // set up input LDSymbol
  input_sym->setFragmentRef(pFragmentRef);
  input_sym->setValue(pValue);

  LDSymbol* output_sym = resolved_result.info->outSymbol();
  bool has_output_sym = (NULL != output_sym);
  if (!resolved_result.existent || !has_output_sym) {
    // it is a new symbol, the output_sym should be NULL.
    assert(NULL == output_sym);

    // if it is a new symbol, create a LDSymbol for the output
    output_sym = m_LDSymbolFactory.allocate();
    new (output_sym) LDSymbol();

    // set up the relation between output LDSymbol and its ResolveInfo
    output_sym->setResolveInfo(*resolved_result.info);
    resolved_result.info->setSymPtr(output_sym);
  }

  if (resolved_result.overriden || !has_output_sym) {
    // symbol can be overriden only if it exists.
    assert(output_sym != NULL);

    // should override output LDSymbol
    output_sym->setFragmentRef(pFragmentRef);
    output_sym->setValue(pValue);
  }

  // After symbol resolution, visibility is changed to the most restrict one.
  // we need to arrange its position in the output symbol .
  if (pType != ResolveInfo::Section) {
    if (!has_output_sym) {
      // We merge sections when reading them. So we do not need to output symbols
      // with section type

      // No matter the symbol is already in the output or not, add it if it
      // should be forcefully set local.
      if (shouldForceLocal(*resolved_result.info))
        m_OutputSymbols.forceLocal(*output_sym);
      else {
        // the symbol should not be forcefully local.
        m_OutputSymbols.add(*output_sym);
      }
    }
    else if (resolved_result.overriden) {
      if (!shouldForceLocal(old_info) ||
          !shouldForceLocal(*resolved_result.info)) {
        // If the old info and the new info are both forcefully local, then
        // we should keep the output_sym in forcefully local category. Else,
        // we should re-sort the output_sym
        m_OutputSymbols.arrange(*output_sym, old_info);
      }
    }
  }

  return input_sym;
}

/// addSymbolFromDynObj - add a symbol from object file and resolve it
/// immediately
LDSymbol* MCLinker::addSymbolFromDynObj(const llvm::StringRef& pName,
                                        ResolveInfo::Type pType,
                                        ResolveInfo::Desc pDesc,
                                        ResolveInfo::Binding pBinding,
                                        ResolveInfo::SizeType pSize,
                                        LDSymbol::ValueType pValue,
                                        MCFragmentRef* pFragmentRef,
                                        ResolveInfo::Visibility pVisibility)
{
  // We merge sections when reading them. So we do not need symbols with
  // section type
  if (pType == ResolveInfo::Section)
    return NULL;

  // ignore symbols with local binding or that have internal or hidden
  // visibility
  if (pBinding == ResolveInfo::Local ||
      pVisibility == ResolveInfo::Internal ||
      pVisibility == ResolveInfo::Hidden)
    return NULL;

  // A protected symbol in a shared library must be treated as a
  // normal symbol when viewed from outside the shared library.
  if (pVisibility == ResolveInfo::Protected)
    pVisibility = ResolveInfo::Default;

  // insert symbol and resolve it immediately
  // resolved_result is a triple <resolved_info, existent, override>
  Resolver::Result resolved_result;
  m_StrSymPool.insertSymbol(pName, true, pType, pDesc, pBinding, pSize, pVisibility,
                            NULL, resolved_result);

  // the return ResolveInfo should not NULL
  assert(NULL != resolved_result.info);

  // create a LDSymbol for the input file.
  LDSymbol* input_sym = m_LDSymbolFactory.allocate();
  new (input_sym) LDSymbol();

  // set up the relation between input LDSymbol and its ResolveInfo
  input_sym->setResolveInfo(*resolved_result.info);

  // set up input LDSymbol
  input_sym->setFragmentRef(pFragmentRef);
  input_sym->setValue(pValue);

  LDSymbol* output_sym = NULL;
  if (!resolved_result.existent) {
    // we get a new symbol, leave it as NULL
    resolved_result.info->setSymPtr(NULL);
  }
  else {
    // we saw the symbol before, but the output_sym still may be NULL.
    output_sym = resolved_result.info->outSymbol();
  }

  if (output_sym != NULL) {
    // After symbol resolution, visibility is changed to the most restrict one.
    // If we are not doing incremental linking, then any symbol with hidden
    // or internal visibility is forcefully set as a local symbol.
    if (shouldForceLocal(*resolved_result.info)) {
      m_OutputSymbols.forceLocal(*output_sym);
    }
  }

  return input_sym;
}

/// defineSymbolForcefully - define an output symbol and override it immediately
LDSymbol* MCLinker::defineSymbolForcefully(const llvm::StringRef& pName,
                                           bool pIsDyn,
                                           ResolveInfo::Type pType,
                                           ResolveInfo::Desc pDesc,
                                           ResolveInfo::Binding pBinding,
                                           ResolveInfo::SizeType pSize,
                                           LDSymbol::ValueType pValue,
                                           MCFragmentRef* pFragmentRef,
                                           ResolveInfo::Visibility pVisibility)
{
  ResolveInfo* info = m_StrSymPool.findInfo(pName);
  LDSymbol* output_sym = NULL;
  if (NULL == info) {
    // the symbol is not in the pool, create a new one.
    // create a ResolveInfo
    Resolver::Result result;
    m_StrSymPool.insertSymbol(pName, pIsDyn, pType, pDesc, pBinding, pSize, pVisibility,
                              NULL, result);
    assert(!result.existent);

    // create a output LDSymbol
    output_sym = m_LDSymbolFactory.allocate();
    new (output_sym) LDSymbol();

    output_sym->setResolveInfo(*result.info);
    result.info->setSymPtr(output_sym);

    if (shouldForceLocal(*result.info))
      m_OutputSymbols.forceLocal(*output_sym);
    else
      m_OutputSymbols.add(*output_sym);
  }
  else {
    // the symbol is already in the pool, override it
    ResolveInfo old_info;
    old_info.override(*info);

    info->setSource(pIsDyn);
    info->setType(pType);
    info->setDesc(pDesc);
    info->setBinding(pBinding);
    info->setVisibility(pVisibility);
    info->setIsSymbol(true);
    info->setSize(pSize);

    output_sym = info->outSymbol();
    if (NULL != output_sym)
      m_OutputSymbols.arrange(*output_sym, old_info);
    else {
      // create a output LDSymbol
      output_sym = m_LDSymbolFactory.allocate();
      new (output_sym) LDSymbol();

      output_sym->setResolveInfo(*info);
      info->setSymPtr(output_sym);

      m_OutputSymbols.add(*output_sym);
    }
  }

  if (NULL != output_sym) {
    output_sym->setFragmentRef(pFragmentRef);
    output_sym->setValue(pValue);
  }

  return output_sym;
}

/// defineSymbolAsRefered - define an output symbol and override it immediately
LDSymbol* MCLinker::defineSymbolAsRefered(const llvm::StringRef& pName,
                                           bool pIsDyn,
                                           ResolveInfo::Type pType,
                                           ResolveInfo::Desc pDesc,
                                           ResolveInfo::Binding pBinding,
                                           ResolveInfo::SizeType pSize,
                                           LDSymbol::ValueType pValue,
                                           MCFragmentRef* pFragmentRef,
                                           ResolveInfo::Visibility pVisibility)
{
  ResolveInfo* info = m_StrSymPool.findInfo(pName);

  if (NULL == info || !info->isUndef()) {
    // only undefined symbol can make a reference.
    return NULL;
  }

  // the symbol is already in the pool, override it
  ResolveInfo old_info;
  old_info.override(*info);

  info->setSource(pIsDyn);
  info->setType(pType);
  info->setDesc(pDesc);
  info->setBinding(pBinding);
  info->setVisibility(pVisibility);
  info->setIsSymbol(true);
  info->setSize(pSize);

  LDSymbol* output_sym = info->outSymbol();
  if (NULL != output_sym) {
    output_sym->setFragmentRef(pFragmentRef);
    output_sym->setValue(pValue);
    m_OutputSymbols.arrange(*output_sym, old_info);
  }
  else {
    // create a output LDSymbol
    output_sym = m_LDSymbolFactory.allocate();
    new (output_sym) LDSymbol();

    output_sym->setResolveInfo(*info);
    info->setSymPtr(output_sym);

    m_OutputSymbols.add(*output_sym);
  }

  return output_sym;
}

/// defineAndResolveSymbolForcefully - define an output symbol and resolve it
/// immediately
LDSymbol* MCLinker::defineAndResolveSymbolForcefully(const llvm::StringRef& pName,
                                                     bool pIsDyn,
                                                     ResolveInfo::Type pType,
                                                     ResolveInfo::Desc pDesc,
                                                     ResolveInfo::Binding pBinding,
                                                     ResolveInfo::SizeType pSize,
                                                     LDSymbol::ValueType pValue,
                                                     MCFragmentRef* pFragmentRef,
                                                     ResolveInfo::Visibility pVisibility)
{
  // Result is <info, existent, override>
  Resolver::Result result;
  ResolveInfo old_info;
  m_StrSymPool.insertSymbol(pName, pIsDyn, pType, pDesc, pBinding, pSize, pVisibility,
                            &old_info, result);

  LDSymbol* output_sym = result.info->outSymbol();
  bool has_output_sym = (NULL != output_sym);

  if (!result.existent || !has_output_sym) {
    output_sym = m_LDSymbolFactory.allocate();
    new (output_sym) LDSymbol();
    output_sym->setResolveInfo(*result.info);
    result.info->setSymPtr(output_sym);
  }

  if (result.overriden || !has_output_sym) {
    output_sym->setFragmentRef(pFragmentRef);
    output_sym->setValue(pValue);
  }

  // After symbol resolution, the visibility is changed to the most restrict.
  // arrange the output position
  if (shouldForceLocal(*result.info))
    m_OutputSymbols.forceLocal(*output_sym);
  else if (has_output_sym)
    m_OutputSymbols.arrange(*output_sym, old_info);
  else
    m_OutputSymbols.add(*output_sym);

  return output_sym;
}

/// defineAndResolveSymbolAsRefered - define an output symbol and resolve it
/// immediately.
LDSymbol* MCLinker::defineAndResolveSymbolAsRefered(const llvm::StringRef& pName,
                                                    bool pIsDyn,
                                                    ResolveInfo::Type pType,
                                                    ResolveInfo::Desc pDesc,
                                                    ResolveInfo::Binding pBinding,
                                                    ResolveInfo::SizeType pSize,
                                                    LDSymbol::ValueType pValue,
                                                    MCFragmentRef* pFragmentRef,
                                                    ResolveInfo::Visibility pVisibility)
{
  ResolveInfo* info = m_StrSymPool.findInfo(pName);

  if (NULL == info || !info->isUndef()) {
    // only undefined symbol can make a reference
    return NULL;
  }

  return defineAndResolveSymbolForcefully(pName,
                                          pIsDyn,
                                          pType,
                                          pDesc,
                                          pBinding,
                                          pSize,
                                          pValue,
                                          pFragmentRef,
                                          pVisibility);
}

/// createSectHdr - create the input section header
LDSection& MCLinker::createSectHdr(const std::string& pName,
                                   LDFileFormat::Kind pKind,
                                   uint32_t pType,
                                   uint32_t pFlag)
{
  // for user such as reader, standard/target fromat
  LDSection* result =
    m_LDSectHdrFactory.produce(pName, pKind, pType, pFlag);

  // check if we need to create a output section for output LDContext
  std::string sect_name = m_SectionMap.getOutputSectName(pName);
  LDSection* output_sect = m_Output.getSection(sect_name);

  if (NULL == output_sect) {
  // create a output section and push it into output LDContext
    output_sect =
      m_LDSectHdrFactory.produce(sect_name, pKind, pType, pFlag);
    m_Output.getSectionTable().push_back(output_sect);
    m_SectionMerger.addMapping(pName, output_sect);
  }
  return *result;
}

/// getOrCreateOutputSectHdr - for reader and standard/target format to get
/// or create the output's section header
LDSection& MCLinker::getOrCreateOutputSectHdr(const std::string& pName,
                                              LDFileFormat::Kind pKind,
                                              uint32_t pType,
                                              uint32_t pFlag,
                                              uint32_t pAlign)
{
  // check if we need to create a output section for output LDContext
  std::string sect_name = m_SectionMap.getOutputSectName(pName);
  LDSection* output_sect = m_Output.getSection(sect_name);

  if (NULL == output_sect) {
  // create a output section and push it into output LDContext
    output_sect =
      m_LDSectHdrFactory.produce(sect_name, pKind, pType, pFlag);
    output_sect->setAlign(pAlign);
    m_Output.getSectionTable().push_back(output_sect);
    m_SectionMerger.addMapping(pName, output_sect);
  }
  return *output_sect;
}

/// getOrCreateSectData - get or create MCSectionData
/// pSection is input LDSection
llvm::MCSectionData& MCLinker::getOrCreateSectData(LDSection& pSection)
{
  // if there is already a section data pointed by section, return it.
  llvm::MCSectionData* sect_data = pSection.getSectionData();
  if (NULL != sect_data) {
    m_Layout.addInputRange(*sect_data, pSection);
    return *sect_data;
  }

  // try to get one from output LDSection
  LDSection* output_sect =
    m_SectionMerger.getOutputSectHdr(pSection.name());

  assert(NULL != output_sect);

  sect_data = output_sect->getSectionData();

  if (NULL != sect_data) {
    pSection.setSectionData(sect_data);
    m_Layout.addInputRange(*sect_data, pSection);
    return *sect_data;
  }

  // if the output LDSection also has no MCSectionData, then create one.
  sect_data = m_LDSectDataFactory.allocate();
  new (sect_data) llvm::MCSectionData(*output_sect);
  pSection.setSectionData(sect_data);
  output_sect->setSectionData(sect_data);
  m_Layout.addInputRange(*sect_data, pSection);
  return *sect_data;
}

/// addRelocation - add a relocation entry in MCLinker (only for object file)
///
/// All symbols should be read and resolved before calling this function.
Relocation* MCLinker::addRelocation(Relocation::Type pType,
                                    const LDSymbol& pSym,
                                    ResolveInfo& pResolveInfo,
                                    MCFragmentRef& pFragmentRef,
                                    Relocation::Address pAddend)
{
  Relocation* relocation = m_Backend.getRelocFactory()->produce(pType,
                                                                pFragmentRef,
                                                                pAddend);

  relocation->setSymInfo(&pResolveInfo);

  m_RelocationList.push_back(relocation);

  m_Backend.scanRelocation(*relocation, pSym, *this, m_Info,
                           m_Info.output());

  return relocation;
}

bool MCLinker::applyRelocations()
{
  RelocationListType::iterator relocIter, relocEnd = m_RelocationList.end();

  for (relocIter = m_RelocationList.begin(); relocIter != relocEnd; ++relocIter) {
    llvm::MCFragment* frag = (llvm::MCFragment*)relocIter;
    static_cast<Relocation*>(frag)->apply(*m_Backend.getRelocFactory(), m_Info);
  }
  return true;
}

void MCLinker::syncRelocationResult()
{

  m_Info.output().memArea()->clean();
  MemoryRegion* region = m_Info.output().memArea()->request(0,
                              m_Info.output().memArea()->size());

  uint8_t* data = region->getBuffer();

  RelocationListType::iterator relocIter, relocEnd = m_RelocationList.end();
  for (relocIter = m_RelocationList.begin(); relocIter != relocEnd; ++relocIter) {

    llvm::MCFragment* frag = (llvm::MCFragment*)relocIter;
    Relocation* reloc = static_cast<Relocation*>(frag);

    // get output file offset
    size_t out_offset = m_Layout.getOutputLDSection(*reloc->targetRef().frag())->offset() +
                        m_Layout.getOutputOffset(reloc->targetRef());

    uint8_t* target_addr = data + out_offset;
    // byte swapping if target and host has different endian, and then write back
    if(llvm::sys::isLittleEndianHost() != m_Backend.isLittleEndian()) {
       uint64_t tmp_data = 0;

       switch(m_Backend.bitclass()) {
         case 32u:
           tmp_data = bswap32(reloc->target());
           std::memcpy(target_addr, &tmp_data, 4);
           break;

         case 64u:
           tmp_data = bswap64(reloc->target());
           std::memcpy(target_addr, &tmp_data, 8);
           break;

         default:
           break;
      }
    }
    else {
      std::memcpy(target_addr, &reloc->target(), m_Backend.bitclass()/8);
    }
  } // end of for

  m_Info.output().memArea()->sync();
}


bool MCLinker::layout()
{
  return m_Layout.layout(m_Info.output(), m_Backend);
}

bool MCLinker::finalizeSymbols()
{
  SymbolCategory::iterator symbol, symEnd = m_OutputSymbols.end();
  for (symbol = m_OutputSymbols.begin(); symbol != symEnd; ++symbol) {

    if (0x0 != (*symbol)->resolveInfo()->reserved()) {
      // if the symbol is target reserved, target backend is responsible
      // for finalizing the value.
      // if target backend does not know this symbol, it will return false
      // and we have to take over the symbol.
      if (m_Backend.finalizeSymbol(**symbol))
        continue;
    }

    if ((*symbol)->resolveInfo()->isAbsolute() ||
        (*symbol)->resolveInfo()->type() == ResolveInfo::File) {
      // absolute symbols and symbols with function type should have
      // zero value
      (*symbol)->setValue(0x0);
      continue;
    }

    if ((*symbol)->hasFragRef()) {
      // set the virtual address of the symbol. If the output file is
      // relocatable object file, the section's virtual address becomes zero.
      // And the symbol's value become section relative offset.
      uint64_t value = getLayout().getOutputOffset(*(*symbol)->fragRef());
      assert(NULL != (*symbol)->fragRef()->frag());
      uint64_t addr  = getLayout().getOutputLDSection(*(*symbol)->fragRef()->frag())->addr();
      (*symbol)->setValue(value + addr);
      continue;
    }
  }

  return true;
}

bool MCLinker::shouldForceLocal(const ResolveInfo& pInfo) const
{
  // forced local symbol matches all rules:
  // 1. We are not doing incremental linking.
  // 2. The symbol is with Hidden or Internal visibility.
  // 3. The symbol should be global or weak. Otherwise, local symbol is local.
  // 4. The symbol is defined or common
  if (m_Info.output().type() != Output::Object &&
      (pInfo.visibility() == ResolveInfo::Hidden ||
         pInfo.visibility() == ResolveInfo::Internal) &&
      (pInfo.isGlobal() || pInfo.isWeak()) &&
      (pInfo.isDefine() || pInfo.isCommon()))
    return true;
  return false;
}

