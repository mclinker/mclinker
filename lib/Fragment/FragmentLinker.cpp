//===- FragmentLinker.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the FragmentLinker class
//
//===----------------------------------------------------------------------===//
#include <mcld/Fragment/FragmentLinker.h>

#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Casting.h>

#include <mcld/LinkerConfig.h>
#include <mcld/Module.h>
#include <mcld/MC/MCLDInput.h>
#include <mcld/LD/Resolver.h>
#include <mcld/LD/LDContext.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/RelocationFactory.h>
#include <mcld/LD/RelocationData.h>
#include <mcld/LD/SectionMerger.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/FileHandle.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Target/TargetLDBackend.h>

using namespace mcld;

/// Constructor
FragmentLinker::FragmentLinker(const LinkerConfig& pConfig,
                               Module& pModule,
                               TargetLDBackend& pBackend)

  : m_Config(pConfig),
    m_Module(pModule),
    m_Backend(pBackend),
    m_LDSymbolFactory(128),
    m_pSectionMerger(NULL),
    m_fCreateOrphan(false)
{
}

/// Destructor
FragmentLinker::~FragmentLinker()
{
  delete m_pSectionMerger;
}

//===----------------------------------------------------------------------===//
// Symbol Operations
//===----------------------------------------------------------------------===//
/// addSymbolFromObject - add a symbol from object file and resolve it
/// immediately
LDSymbol* FragmentLinker::addSymbolFromObject(const llvm::StringRef& pName,
                                        ResolveInfo::Type pType,
                                        ResolveInfo::Desc pDesc,
                                        ResolveInfo::Binding pBinding,
                                        ResolveInfo::SizeType pSize,
                                        LDSymbol::ValueType pValue,
                                        FragmentRef* pFragmentRef,
                                        ResolveInfo::Visibility pVisibility)
{

  // resolved_result is a triple <resolved_info, existent, override>
  Resolver::Result resolved_result;
  ResolveInfo old_info; // used for arrange output symbols

  if (pBinding == ResolveInfo::Local) {
    // if the symbol is a local symbol, create a LDSymbol for input, but do not
    // resolve them.
    resolved_result.info     = m_Module.getNamePool().createSymbol(pName,
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
    m_Module.getNamePool().insertSymbol(pName, false, pType, pDesc, pBinding,
                                        pSize, pVisibility,
                                        &old_info, resolved_result);
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
        m_Module.getSymbolTable().forceLocal(*output_sym);
      else {
        // the symbol should not be forcefully local.
        m_Module.getSymbolTable().add(*output_sym);
      }
    }
    else if (resolved_result.overriden) {
      if (!shouldForceLocal(old_info) ||
          !shouldForceLocal(*resolved_result.info)) {
        // If the old info and the new info are both forcefully local, then
        // we should keep the output_sym in forcefully local category. Else,
        // we should re-sort the output_sym
        m_Module.getSymbolTable().arrange(*output_sym, old_info);
      }
    }
  }

  return input_sym;
}

/// addSymbolFromDynObj - add a symbol from object file and resolve it
/// immediately
LDSymbol* FragmentLinker::addSymbolFromDynObj(const llvm::StringRef& pName,
                                        ResolveInfo::Type pType,
                                        ResolveInfo::Desc pDesc,
                                        ResolveInfo::Binding pBinding,
                                        ResolveInfo::SizeType pSize,
                                        LDSymbol::ValueType pValue,
                                        FragmentRef* pFragmentRef,
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
  m_Module.getNamePool().insertSymbol(pName, true, pType, pDesc,
                                      pBinding, pSize, pVisibility,
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
      m_Module.getSymbolTable().forceLocal(*output_sym);
    }
  }

  return input_sym;
}

/// defineSymbolForcefully - define an output symbol and override it immediately
LDSymbol* FragmentLinker::defineSymbolForcefully(const llvm::StringRef& pName,
                                           bool pIsDyn,
                                           ResolveInfo::Type pType,
                                           ResolveInfo::Desc pDesc,
                                           ResolveInfo::Binding pBinding,
                                           ResolveInfo::SizeType pSize,
                                           LDSymbol::ValueType pValue,
                                           FragmentRef* pFragmentRef,
                                           ResolveInfo::Visibility pVisibility)
{
  ResolveInfo* info = m_Module.getNamePool().findInfo(pName);
  LDSymbol* output_sym = NULL;
  if (NULL == info) {
    // the symbol is not in the pool, create a new one.
    // create a ResolveInfo
    Resolver::Result result;
    m_Module.getNamePool().insertSymbol(pName, pIsDyn, pType, pDesc,
                                        pBinding, pSize, pVisibility,
                                        NULL, result);
    assert(!result.existent);

    // create a output LDSymbol
    output_sym = m_LDSymbolFactory.allocate();
    new (output_sym) LDSymbol();

    output_sym->setResolveInfo(*result.info);
    result.info->setSymPtr(output_sym);

    if (shouldForceLocal(*result.info))
      m_Module.getSymbolTable().forceLocal(*output_sym);
    else
      m_Module.getSymbolTable().add(*output_sym);
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
      m_Module.getSymbolTable().arrange(*output_sym, old_info);
    else {
      // create a output LDSymbol
      output_sym = m_LDSymbolFactory.allocate();
      new (output_sym) LDSymbol();

      output_sym->setResolveInfo(*info);
      info->setSymPtr(output_sym);

      m_Module.getSymbolTable().add(*output_sym);
    }
  }

  if (NULL != output_sym) {
    output_sym->setFragmentRef(pFragmentRef);
    output_sym->setValue(pValue);
  }

  return output_sym;
}

/// defineSymbolAsRefered - define an output symbol and override it immediately
LDSymbol* FragmentLinker::defineSymbolAsRefered(const llvm::StringRef& pName,
                                           bool pIsDyn,
                                           ResolveInfo::Type pType,
                                           ResolveInfo::Desc pDesc,
                                           ResolveInfo::Binding pBinding,
                                           ResolveInfo::SizeType pSize,
                                           LDSymbol::ValueType pValue,
                                           FragmentRef* pFragmentRef,
                                           ResolveInfo::Visibility pVisibility)
{
  ResolveInfo* info = m_Module.getNamePool().findInfo(pName);

  if (NULL == info || !(info->isUndef() || info->isDyn())) {
    // only undefined symbol and dynamic symbol can make a reference.
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
    m_Module.getSymbolTable().arrange(*output_sym, old_info);
  }
  else {
    // create a output LDSymbol
    output_sym = m_LDSymbolFactory.allocate();
    new (output_sym) LDSymbol();

    output_sym->setResolveInfo(*info);
    info->setSymPtr(output_sym);

    m_Module.getSymbolTable().add(*output_sym);
  }

  return output_sym;
}

/// defineAndResolveSymbolForcefully - define an output symbol and resolve it
/// immediately
LDSymbol* FragmentLinker::defineAndResolveSymbolForcefully(const llvm::StringRef& pName,
                                                     bool pIsDyn,
                                                     ResolveInfo::Type pType,
                                                     ResolveInfo::Desc pDesc,
                                                     ResolveInfo::Binding pBinding,
                                                     ResolveInfo::SizeType pSize,
                                                     LDSymbol::ValueType pValue,
                                                     FragmentRef* pFragmentRef,
                                                     ResolveInfo::Visibility pVisibility)
{
  // Result is <info, existent, override>
  Resolver::Result result;
  ResolveInfo old_info;
  m_Module.getNamePool().insertSymbol(pName, pIsDyn, pType, pDesc, pBinding,
                                      pSize, pVisibility,
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
    m_Module.getSymbolTable().forceLocal(*output_sym);
  else if (has_output_sym)
    m_Module.getSymbolTable().arrange(*output_sym, old_info);
  else
    m_Module.getSymbolTable().add(*output_sym);

  return output_sym;
}

/// defineAndResolveSymbolAsRefered - define an output symbol and resolve it
/// immediately.
LDSymbol* FragmentLinker::defineAndResolveSymbolAsRefered(const llvm::StringRef& pName,
                                                    bool pIsDyn,
                                                    ResolveInfo::Type pType,
                                                    ResolveInfo::Desc pDesc,
                                                    ResolveInfo::Binding pBinding,
                                                    ResolveInfo::SizeType pSize,
                                                    LDSymbol::ValueType pValue,
                                                    FragmentRef* pFragmentRef,
                                                    ResolveInfo::Visibility pVisibility)
{
  ResolveInfo* info = m_Module.getNamePool().findInfo(pName);

  if (NULL == info || !(info->isUndef() || info->isDyn())) {
    // only undefined symbol and dynamic symbol can make a reference.
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

bool FragmentLinker::finalizeSymbols()
{
  Module::sym_iterator symbol, symEnd = m_Module.sym_end();
  for (symbol = m_Module.sym_begin(); symbol != symEnd; ++symbol) {

    if ((*symbol)->resolveInfo()->isAbsolute() ||
        (*symbol)->resolveInfo()->type() == ResolveInfo::File) {
      // absolute symbols or symbols with function type should have
      // zero value
      (*symbol)->setValue(0x0);
      continue;
    }

    if ((*symbol)->resolveInfo()->type() == ResolveInfo::ThreadLocal) {
      m_Backend.finalizeTLSSymbol(*this, **symbol);
      continue;
    }

    if ((*symbol)->hasFragRef()) {
      // set the virtual address of the symbol. If the output file is
      // relocatable object file, the section's virtual address becomes zero.
      // And the symbol's value become section relative offset.
      uint64_t value = (*symbol)->fragRef()->getOutputOffset();
      assert(NULL != (*symbol)->fragRef()->frag());
      uint64_t addr  = getLayout().getOutputLDSection(*(*symbol)->fragRef()->frag())->addr();
      (*symbol)->setValue(value + addr);
      continue;
    }
  }

  // finialize target-dependent symbols
  return m_Backend.finalizeSymbols(*this);
}

bool FragmentLinker::shouldForceLocal(const ResolveInfo& pInfo) const
{
  // forced local symbol matches all rules:
  // 1. We are not doing incremental linking.
  // 2. The symbol is with Hidden or Internal visibility.
  // 3. The symbol should be global or weak. Otherwise, local symbol is local.
  // 4. The symbol is defined or common
  if (LinkerConfig::Object != m_Config.codeGenType() &&
      (pInfo.visibility() == ResolveInfo::Hidden ||
         pInfo.visibility() == ResolveInfo::Internal) &&
      (pInfo.isGlobal() || pInfo.isWeak()) &&
      (pInfo.isDefine() || pInfo.isCommon()))
    return true;
  return false;
}

//===----------------------------------------------------------------------===//
// Section Operations
//===----------------------------------------------------------------------===//
/// createSectHdr - create the input section header
LDSection& FragmentLinker::createSectHdr(const std::string& pName,
                                   LDFileFormat::Kind pKind,
                                   uint32_t pType,
                                   uint32_t pFlag)
{
  // for user such as reader, standard/target fromat
  LDSection* result = LDSection::Create(pName, pKind, pType, pFlag);

  const SectionMap::NamePair& pair = m_Config.scripts().sectionMap().find(pName);
  std::string output_name = (pair.isNull())?pName:pair.to;
  LDSection* output_sect = m_Module.getSection(output_name);
  if (NULL == output_sect) {
    // create a output section and push it into output LDContext
    output_sect = LDSection::Create(output_name, pKind, pType, pFlag);
    m_Module.getSectionTable().push_back(output_sect);
    m_pSectionMerger->append(pName, *output_sect);
  }
  return *result;
}

/// getOrCreateOutputSectHdr - for reader and standard/target format to get
/// or create the output's section header
LDSection& FragmentLinker::getOrCreateOutputSectHdr(const std::string& pName,
                                              LDFileFormat::Kind pKind,
                                              uint32_t pType,
                                              uint32_t pFlag,
                                              uint32_t pAlign)
{
  const SectionMap::NamePair& pair = m_Config.scripts().sectionMap().find(pName);
  std::string output_name = (pair.isNull())?pName:pair.to;
  LDSection* output_sect = m_Module.getSection(output_name);
  if (NULL == output_sect) {
    output_sect = LDSection::Create(output_name, pKind, pType, pFlag);
    output_sect->setAlign(pAlign);
    m_Module.getSectionTable().push_back(output_sect);
    m_pSectionMerger->append(pName, *output_sect);
  }
  return *output_sect;
}

/// getOrCreateInputSectData - get or create SectionData
/// pSection is input LDSection
SectionData&
FragmentLinker::getOrCreateInputSectData(LDSection& pSection)
{
  // if there is already a section data pointed by section, return it.
  SectionData* sect_data = pSection.getSectionData();
  if (NULL != sect_data) {
    m_Layout.addInputRange(*sect_data, pSection);
    return *sect_data;
  }

  // try to get one from output LDSection
  LDSection* output_sect =
    m_pSectionMerger->getMatchedSection(pSection.name());

  assert(NULL != output_sect);

  sect_data = output_sect->getSectionData();

  if (NULL != sect_data) {
    pSection.setSectionData(sect_data);
    m_Layout.addInputRange(*sect_data, pSection);
    return *sect_data;
  }

  // if the output LDSection also has no SectionData, then create one.
  sect_data = SectionData::Create(*output_sect);
  pSection.setSectionData(sect_data);
  output_sect->setSectionData(sect_data);
  m_Layout.addInputRange(*sect_data, pSection);
  return *sect_data;
}

/// getOrCreateOutputSectData - get or create SectionData
/// pSection is output LDSection
SectionData&
FragmentLinker::getOrCreateOutputSectData(LDSection& pSection)
{
  // if there is already a section data pointed by section, return it.
  SectionData* sect_data = pSection.getSectionData();
  if (NULL != sect_data) {
    m_Layout.addInputRange(*sect_data, pSection);
    return *sect_data;
  }

  // try to get one from output LDSection
  LDSection* output_sect =
    m_pSectionMerger->getMatchedSection(pSection.name());

  assert(NULL != output_sect);

  sect_data = output_sect->getSectionData();

  if (NULL != sect_data) {
    pSection.setSectionData(sect_data);
    m_Layout.addInputRange(*sect_data, pSection);
    return *sect_data;
  }

  // if the output LDSection also has no SectionData, then create one.
  sect_data = SectionData::Create(*output_sect);
  pSection.setSectionData(sect_data);
  output_sect->setSectionData(sect_data);
  m_Layout.addInputRange(*sect_data, pSection);
  return *sect_data;
}


RelocationData& FragmentLinker::getOrCreateInputRelocData(LDSection& pSection)
{
  // if there is already a relocation data pointed by section, return it
  RelocationData* reloc_data = pSection.getRelocationData();
  if (NULL != reloc_data) {
    return *reloc_data;
  }
  // otherwise, create one and push it into Module's RelocDataTable
  reloc_data = RelocationData::Create(pSection);
  pSection.setRelocationData(reloc_data);
  m_Module.getRelocationDataTable().push_back(reloc_data);
  return *reloc_data;
}

RelocationData& FragmentLinker::getOrCreateOutputRelocData(LDSection& pSection)
{
  // if there is already a relocation data pointed by section, return it
  RelocationData* reloc_data = pSection.getRelocationData();
  if (NULL != reloc_data) {
    return *reloc_data;
  }
  // otherwise, create one
  reloc_data = RelocationData::Create(pSection);
  pSection.setRelocationData(reloc_data);
  return *reloc_data;
}

RelocationData& FragmentLinker::getOrCreateOrphanRelocData()
{
  static RelocationData* reloc_data = RelocationData::Create();

  // the first time we call this function, push the orphan relocation data into
  // Module's RelocDataTable
  if (!m_fCreateOrphan) {
    m_Module.getRelocationDataTable().push_back(reloc_data);
    m_fCreateOrphan = true;
  }
  return *reloc_data;
}

void FragmentLinker::initSectionMap()
{
  if (NULL == m_pSectionMerger) {
    m_pSectionMerger = new SectionMerger(m_Config, m_Module);
    m_pSectionMerger->initOutputSectMap();
  }
}

bool FragmentLinker::layout()
{
  return m_Layout.layout(m_Module, m_Backend, m_Config);
}

//===----------------------------------------------------------------------===//
// Relocation Operations
//===----------------------------------------------------------------------===//
/// addRelocation - add a relocation entry in FragmentLinker (only for object file)
///
/// All symbols should be read and resolved before calling this function.
Relocation* FragmentLinker::addRelocation(Relocation::Type pType,
                                          const LDSymbol& pSym,
                                          ResolveInfo& pResolveInfo,
                                          FragmentRef& pFragmentRef,
                                          LDSection* pSection,
                                          const LDSection& pTargetSection,
                                          Relocation::Address pAddend)
{
  // FIXME: we should dicard sections and symbols first instead
  // if the symbol is in the discarded input section, then we also need to
  // discard this relocation.
  if (pSym.fragRef() == NULL &&
      pResolveInfo.type() == ResolveInfo::Section &&
      pResolveInfo.desc() == ResolveInfo::Undefined)
    return NULL;

  Relocation* relocation = m_Backend.getRelocFactory()->produce(pType,
                                                                pFragmentRef,
                                                                pAddend);
  relocation->setSymInfo(&pResolveInfo);

  // push relocation into the input RelocationData
  RelocationData* reloc_data = NULL;
  if (NULL != pSection)
    reloc_data = &getOrCreateInputRelocData(*pSection);
  else
    reloc_data = &getOrCreateOrphanRelocData();
  reloc_data->getFragmentList().push_back(relocation);

  // scan relocation
  if (LinkerConfig::Object != m_Config.codeGenType())
    m_Backend.scanRelocation(*relocation, pSym, *this, pTargetSection);
  else
    m_Backend.partialScanRelocation(*relocation, pSym, pTargetSection);
  return relocation;
}

bool FragmentLinker::applyRelocations()
{
  // when producing relocatables, no need to apply relocation
  if (LinkerConfig::Object == m_Config.codeGenType())
    return true;

  Module::reloc_data_iterator dataIter, dataEnd = m_Module.reloc_data_end();
  for (dataIter = m_Module.reloc_data_begin(); dataIter != dataEnd; ++dataIter) {
    RelocationData* reloc_data = *dataIter;
    RelocationData::iterator relocIter, reloc_end = reloc_data->end();

    for (relocIter = reloc_data->begin(); relocIter != reloc_end; ++relocIter) {
      Relocation* reloc = llvm::cast<Relocation>(relocIter);
      reloc->apply(*m_Backend.getRelocFactory());
    }
  }
  return true;
}


void FragmentLinker::syncRelocationResult(MemoryArea& pOutput)
{
  if (LinkerConfig::Object != m_Config.codeGenType())
    normalSyncRelocationResult(pOutput);
  else
    partialSyncRelocationResult(pOutput);
  return;
}

void FragmentLinker::normalSyncRelocationResult(MemoryArea& pOutput)
{
  MemoryRegion* region = pOutput.request(0, pOutput.handler()->size());

  uint8_t* data = region->getBuffer();

  Module::reloc_data_iterator dataIter, dataEnd = m_Module.reloc_data_end();
  for (dataIter = m_Module.reloc_data_begin(); dataIter != dataEnd; ++dataIter) {
    RelocationData* reloc_data = *dataIter;
    RelocationData::iterator relocIter, relocEnd = reloc_data->end();

    for (relocIter = reloc_data->begin(); relocIter != relocEnd; ++relocIter) {

      Relocation* reloc = llvm::cast<Relocation>(relocIter);

      // get output file offset
      size_t out_offset = m_Layout.getOutputLDSection(*reloc->targetRef().frag())->offset() +
                          reloc->targetRef().getOutputOffset();

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
    }
  }

  pOutput.clear();
}

void FragmentLinker::partialSyncRelocationResult(MemoryArea& pOutput)
{

  MemoryRegion* region = pOutput.request(0, pOutput.handler()->size());

  uint8_t* data = region->getBuffer();

  // traver outputs LDSection to get RelocationData
  Module::iterator sectIter, sectEnd = m_Module.end();
  for (sectIter = m_Module.begin(); sectIter != sectEnd; ++sectIter) {
    if (LDFileFormat::Relocation != (*sectIter)->kind())
      continue;
    RelocationData* reloc_data = (*sectIter)->getRelocationData();
    RelocationData::iterator relocIter, relocEnd = reloc_data->end();

    for (relocIter = reloc_data->begin(); relocIter != relocEnd; ++relocIter) {

      Relocation* reloc = llvm::cast<Relocation>(relocIter);

      // get output file offset
      size_t out_offset = m_Layout.getOutputLDSection(*reloc->targetRef().frag())->offset() +
                          reloc->targetRef().getOutputOffset();

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
    }
  }


  pOutput.clear();
}

/// isOutputPIC - return whether the output is position-independent
bool FragmentLinker::isOutputPIC() const
{
  static bool result = checkIsOutputPIC();
  return result;
}

/// isStaticLink - return whether we're doing static link
bool FragmentLinker::isStaticLink() const
{
  static bool result = checkIsStaticLink();
  return result;
}

bool FragmentLinker::checkIsOutputPIC() const
{
  if (LinkerConfig::DynObj == m_Config.codeGenType() ||
      m_Config.options().isPIE())
    return true;
  return false;
}

bool FragmentLinker::checkIsStaticLink() const
{
  if (m_Module.getLibraryList().empty())
    return true;
  return false;
}

