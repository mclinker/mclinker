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
    // if the symbol is not a local symbol, insert and resolve it immediately
    m_StrSymPool.insertSymbol(pName, false, pType, pDesc, pBinding, pSize,
                              pVisibility, resolved_result);
  }

  // the return ResolveInfo should not NULL
  assert(NULL != resolved_result.info);

  // create a LDSymbol for the input file.
  LDSymbol* input_sym = m_LDSymbolFactory.allocate();
  new (input_sym) LDSymbol();
  input_sym->setResolveInfo(*resolved_result.info);
  input_sym->setFragmentRef(pFragmentRef);
  input_sym->setValue(pValue);

  LDSymbol* output_sym = resolved_result.info->outSymbol();
  bool in_output = (NULL != output_sym);
  if (!resolved_result.existent || !in_output) {
    // if it is a new symbol, create a LDSymbol for the output
    output_sym = m_LDSymbolFactory.allocate();
    new (output_sym) LDSymbol();
    output_sym->setResolveInfo(*resolved_result.info);
    resolved_result.info->setSymPtr(output_sym);

    if (pType != ResolveInfo::Section) {
      // We merge sections when reading them. So we do not need symbols with
      // section type
      m_OutputSymbols.add(*output_sym);
    }
  }

  if (resolved_result.overriden || !in_output) {
    // should override output LDSymbol
    output_sym->setFragmentRef(pFragmentRef);
    output_sym->setValue(pValue);
  }

  // If we are not doing incremental linking, then any symbol with hidden
  // or internal visibility is forcefully set as a local symbol.
  if ((resolved_result.info->visibility() == ResolveInfo::Hidden ||
       resolved_result.info->visibility() == ResolveInfo::Internal) &&
      (resolved_result.info->isGlobal() || resolved_result.info->isWeak())) {
    if (shouldForceLocal(*resolved_result.info)) {
      m_OutputSymbols.forceLocal(*output_sym);
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
                            resolved_result);

  // the return ResolveInfo should not NULL
  assert(NULL != resolved_result.info);

  // create a LDSymbol for the input file.
  LDSymbol* input_sym = m_LDSymbolFactory.allocate();
  new (input_sym) LDSymbol();
  input_sym->setResolveInfo(*resolved_result.info);
  input_sym->setFragmentRef(pFragmentRef);
  input_sym->setValue(pValue);

  LDSymbol* output_sym = NULL;
  if (!resolved_result.existent) {
    // we get a new symbol, leave it as NULL
    resolved_result.info->setSymPtr(NULL);
  }
  else {
    // we saw the symbol before.
    output_sym = resolved_result.info->outSymbol();
  }

  // If we are not doing incremental linking, then any symbol with hidden
  // or internal visibility is forcefully set as a local symbol.
  if ((resolved_result.info->visibility() == ResolveInfo::Hidden ||
       resolved_result.info->visibility() == ResolveInfo::Internal) &&
      (resolved_result.info->isGlobal() || resolved_result.info->isWeak())) {
    if (NULL != output_sym && shouldForceLocal(*resolved_result.info)) {
      m_OutputSymbols.forceLocal(*output_sym);
    }
  }

  return input_sym;
}

/// defineSymbolForcefully - define an output symbol and resolve it immediately
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
  // Result is <info, existent, override>
  Resolver::Result result;
  m_StrSymPool.insertSymbol(pName, pIsDyn, pType, pDesc, pBinding, pSize, pVisibility,
                            result);

  LDSymbol* output_sym = result.info->outSymbol();
  bool in_output = (NULL != output_sym);
  if (!result.existent || !in_output) {
    output_sym = m_LDSymbolFactory.allocate();
    new (output_sym) LDSymbol();
    output_sym->setResolveInfo(*result.info);
    result.info->setSymPtr(output_sym);
  }

  if (result.overriden || !in_output) {
    output_sym->setFragmentRef(pFragmentRef);
    if (result.info->isAbsolute())
      output_sym->setValue(pValue);
  }

  if (!result.existent && result.info->isLocal()) {
    // if this symbol is a new symbol, and the binding is local, try to
    // forcefully set as a local symbol.
    // @ref Google gold linker: symtab.cc :1764
    if (shouldForceLocal(*result.info)) {
      m_OutputSymbols.forceLocal(*output_sym);
    }
  }

  return output_sym;
}

/// defineSymbolAsRefered - define an output symbol and resolve it immediately
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
    // only undefined symbol can make a reference
    return NULL;
  }

  return defineSymbolForcefully(pName,
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
    output_sect->setIndex(m_Output.getSectionTable().size());
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
                                              uint32_t pFlag)
{
  // check if we need to create a output section for output LDContext
  std::string sect_name = m_SectionMap.getOutputSectName(pName);
  LDSection* output_sect = m_Output.getSection(sect_name);

  if (NULL == output_sect) {
  // create a output section and push it into output LDContext
    output_sect =
      m_LDSectHdrFactory.produce(sect_name, pKind, pType, pFlag);
    output_sect->setIndex(m_Output.getSectionTable().size());
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
                                    ResolveInfo& pResolveInfo,
                                    MCFragmentRef& pFragmentRef,
                                    Relocation::Address pAddend)
{
  Relocation* relocation = m_Backend.getRelocFactory()->produce(pType,
                                                                pFragmentRef,
                                                                pAddend);

  relocation->setSymInfo(&pResolveInfo);

  m_RelocationList.push_back(relocation);

  m_Backend.scanRelocation(*relocation, *this, m_Info,
                           m_Info.output().type());

  return relocation;
}

bool MCLinker::applyRelocations()
{
  RelocationListType::iterator relocIter, relocEnd = m_RelocationList.end();

  for (relocIter = m_RelocationList.begin(); relocIter != relocEnd; ++relocIter) {
    llvm::MCFragment* frag = (llvm::MCFragment*)relocIter;
    static_cast<Relocation*>(frag)->apply();
  }
  return true;
}

void MCLinker::syncRelocationResult() {

  RelocationListType::iterator relocIter, relocEnd = m_RelocationList.end();

  for (relocIter = m_RelocationList.begin(); relocIter != relocEnd; ++relocIter) {

    llvm::MCFragment* frag = (llvm::MCFragment*)relocIter;
    Relocation* reloc = static_cast<Relocation*>(frag);

    // get output file offset
    size_t out_offset = m_Layout.getOutputLDSection(*reloc->targetRef().frag())->offset() +
                        m_Layout.getOutputOffset(reloc->targetRef());

    //request the target region
    MemoryRegion* region = m_Info.output().memArea()->request(out_offset,
                                                       m_Backend.bitclass()/8,
                                                       true);

    // byte swapping if target and host has different endian, and then write back
    if(llvm::sys::isLittleEndianHost() != m_Backend.isLittleEndian()) {
       uint64_t tmp_data = 0;

       switch(m_Backend.bitclass()) {
         case 32u:
           tmp_data = bswap32(reloc->target());
           std::memcpy(region->getBuffer(), &tmp_data, 4);
           break;

         case 64u:
           tmp_data = bswap64(reloc->target());
           std::memcpy(region->getBuffer(), &tmp_data, 8);
           break;

         default:
           break;
      }
    }
    else {
      std::memcpy(region->getBuffer(), &reloc->target(), m_Backend.bitclass()/8);
    }
  } // end of for
  
  m_Info.output().memArea()->sync();
}


bool MCLinker::layout()
{
  return m_Layout.layout(m_Output, m_Backend);
}

bool MCLinker::finalizeSymbols()
{
  SymbolCategory::iterator symbol, symEnd = m_OutputSymbols.end();
  for (symbol = m_OutputSymbols.begin(); symbol != symEnd; ++symbol) {

    if ((*symbol)->resolveInfo()->isAbsolute() ||
        (*symbol)->resolveInfo()->type() == ResolveInfo::File) {
      // absolute symbols and symbols with function type should have
      // zero value
      (*symbol)->setValue(0x0);
      continue;
    }

    if (0x0 != (*symbol)->resolveInfo()->reserved()) {
      // if the symbol is target reserved, target backend is responsible
      // for finalizing the value.
      // if target backend does not know this symbol, it will return false
      // and we have to take over the symbol.
      if (m_Backend.finalizeSymbol(**symbol))
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
  if (pInfo.isDefine() || pInfo.isCommon())
    return true;
  return false;
}
