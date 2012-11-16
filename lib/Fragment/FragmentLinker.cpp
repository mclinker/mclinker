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
#include <mcld/LD/BranchIslandFactory.h>
#include <mcld/LD/Layout.h>
#include <mcld/LD/Resolver.h>
#include <mcld/LD/LDContext.h>
#include <mcld/LD/RelocationFactory.h>
#include <mcld/LD/RelocData.h>
#include <mcld/LD/SectionRules.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MemoryArea.h>
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
    m_Backend(pBackend) {
}

/// Destructor
FragmentLinker::~FragmentLinker()
{
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

  // Step 1. calculate a Resolver::Result
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

  /// Step 2. create an input LDSymbol.
  // create a LDSymbol for the input file.
  LDSymbol* input_sym = LDSymbol::Create(*resolved_result.info);
  input_sym->setFragmentRef(pFragmentRef);
  input_sym->setValue(pValue);

  // Step 3. Set up corresponding output LDSymbol
  LDSymbol* output_sym = resolved_result.info->outSymbol();
  bool has_output_sym = (NULL != output_sym);
  if (!resolved_result.existent || !has_output_sym) {
    // it is a new symbol, the output_sym should be NULL.
    assert(NULL == output_sym);

    if (pType == ResolveInfo::Section) {
      // if it is a section symbol, its output LDSymbol is the input LDSymbol.
      output_sym = input_sym;
    }
    else {
      // if it is a new symbol, create a LDSymbol for the output
      output_sym = LDSymbol::Create(*resolved_result.info);
    }
    resolved_result.info->setSymPtr(output_sym);
  }

  if (resolved_result.overriden || !has_output_sym) {
    // symbol can be overriden only if it exists.
    assert(output_sym != NULL);

    // should override output LDSymbol
    output_sym->setFragmentRef(pFragmentRef);
    output_sym->setValue(pValue);
  }

  // Step 4. Adjust the position of output LDSymbol.
  // After symbol resolution, visibility is changed to the most restrict one.
  // we need to arrange its position in the output symbol. We arrange the
  // positions by sorting symbols in SymbolCategory.
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
  // We don't need sections of dynamic objects. So we ignore section symbols.
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
  LDSymbol* input_sym = LDSymbol::Create(*resolved_result.info);
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
    output_sym = LDSymbol::Create(*result.info);
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
      output_sym = LDSymbol::Create(*info);
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
    output_sym = LDSymbol::Create(*info);
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
    output_sym = LDSymbol::Create(*result.info);
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
      m_Backend.finalizeTLSSymbol(**symbol);
      continue;
    }

    if ((*symbol)->hasFragRef()) {
      // set the virtual address of the symbol. If the output file is
      // relocatable object file, the section's virtual address becomes zero.
      // And the symbol's value become section relative offset.
      uint64_t value = (*symbol)->fragRef()->getOutputOffset();
      assert(NULL != (*symbol)->fragRef()->frag());
      uint64_t addr = (*symbol)->fragRef()->frag()->getParent()->getSection().addr();
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
// Relocation Operations
//===----------------------------------------------------------------------===//
/// addRelocation - add a relocation entry in FragmentLinker (only for object file)
///
/// All symbols should be read and resolved before calling this function.
Relocation* FragmentLinker::addRelocation(Relocation::Type pType,
                                          LDSymbol& pSym,
                                          LDSection& pSection,
                                          uint32_t pOffset,
                                          Relocation::Address pAddend)
{
  // FIXME: we should dicard sections and symbols first instead
  // if the symbol is in the discarded input section, then we also need to
  // discard this relocation.
  ResolveInfo* resolve_info = pSym.resolveInfo();
  if (!pSym.hasFragRef() &&
      resolve_info->type() == ResolveInfo::Section &&
      resolve_info->desc() == ResolveInfo::Undefined)
    return NULL;

  FragmentRef* frag_ref = FragmentRef::Create(*pSection.getLink(), pOffset);

  Relocation* relocation = m_Backend.getRelocFactory()->produce(pType,
                                                                *frag_ref,
                                                                pAddend);

  relocation->setSymInfo(resolve_info);
  pSection.getRelocData()->getFragmentList().push_back(relocation);

  return relocation;
}

bool FragmentLinker::applyRelocations()
{
  // when producing relocatables, no need to apply relocation
  if (LinkerConfig::Object == m_Config.codeGenType())
    return true;

  // apply all relocations of all inputs
  Module::obj_iterator input, inEnd = m_Module.obj_end();
  for (input = m_Module.obj_begin(); input != inEnd; ++input) {
    LDContext::sect_iterator rs, rsEnd = (*input)->context()->relocSectEnd();
    for (rs = (*input)->context()->relocSectBegin(); rs != rsEnd; ++rs) {
      // bypass the reloc section if
      // 1. its section kind is changed to Ignore. (The target section is a
      // discarded group section.)
      // 2. it has no reloc data. (All symbols in the input relocs are in the
      // discarded group sections)
      if (LDFileFormat::Ignore == (*rs)->kind() || !(*rs)->hasRelocData())
        continue;
      RelocData::iterator reloc, rEnd = (*rs)->getRelocData()->end();
      for (reloc = (*rs)->getRelocData()->begin(); reloc != rEnd; ++reloc) {
        Relocation* relocation = llvm::cast<Relocation>(reloc);
        relocation->apply(*m_Backend.getRelocFactory());
      } // for all relocations
    } // for all relocation section
  } // for all inputs

  // apply relocations created by relaxation
  BranchIslandFactory* br_factory = m_Backend.getBRIslandFactory();
  BranchIslandFactory::iterator facIter, facEnd = br_factory->end();
  for (facIter = br_factory->begin(); facIter != facEnd; ++facIter) {
    BranchIsland& island = *facIter;
    BranchIsland::reloc_iterator iter, iterEnd = island.reloc_end();
    for (iter = island.reloc_begin(); iter != iterEnd; ++iter)
      (*iter)->apply(*m_Backend.getRelocFactory());
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

  // sync all relocations of all inputs
  Module::obj_iterator input, inEnd = m_Module.obj_end();
  for (input = m_Module.obj_begin(); input != inEnd; ++input) {
    LDContext::sect_iterator rs, rsEnd = (*input)->context()->relocSectEnd();
    for (rs = (*input)->context()->relocSectBegin(); rs != rsEnd; ++rs) {
      // bypass the reloc section if
      // 1. its section kind is changed to Ignore. (The target section is a
      // discarded group section.)
      // 2. it has no reloc data. (All symbols in the input relocs are in the
      // discarded group sections)
      if (LDFileFormat::Ignore == (*rs)->kind() || !(*rs)->hasRelocData())
        continue;
      RelocData::iterator reloc, rEnd = (*rs)->getRelocData()->end();
      for (reloc = (*rs)->getRelocData()->begin(); reloc != rEnd; ++reloc) {
        Relocation* relocation = llvm::cast<Relocation>(reloc);

        // bypass the relocation with NONE type. This is to avoid overwrite the
        // target result by NONE type relocation if there is a place which has
        // two relocations to apply to, and one of it is NONE type. The result
        // we want is the value of the other relocation result. For example,
        // in .exidx, there are usually an R_ARM_NONE and R_ARM_PREL31 apply to
        // the same place
        if (0x0 == relocation->type())
          continue;
        writeRelocationResult(*relocation, data);
      } // for all relocations
    } // for all relocation section
  } // for all inputs

  // sync relocations created by relaxation
  BranchIslandFactory* br_factory = m_Backend.getBRIslandFactory();
  BranchIslandFactory::iterator facIter, facEnd = br_factory->end();
  for (facIter = br_factory->begin(); facIter != facEnd; ++facIter) {
    BranchIsland& island = *facIter;
    BranchIsland::reloc_iterator iter, iterEnd = island.reloc_end();
    for (iter = island.reloc_begin(); iter != iterEnd; ++iter) {
      Relocation* reloc = *iter;
      writeRelocationResult(*reloc, data);
    }
  }

  pOutput.clear();
}

void FragmentLinker::partialSyncRelocationResult(MemoryArea& pOutput)
{
  MemoryRegion* region = pOutput.request(0, pOutput.handler()->size());

  uint8_t* data = region->getBuffer();

  // traverse outputs' LDSection to get RelocData
  Module::iterator sectIter, sectEnd = m_Module.end();
  for (sectIter = m_Module.begin(); sectIter != sectEnd; ++sectIter) {
    if (LDFileFormat::Relocation != (*sectIter)->kind())
      continue;

    RelocData* reloc_data = (*sectIter)->getRelocData();
    RelocData::iterator relocIter, relocEnd = reloc_data->end();
    for (relocIter = reloc_data->begin(); relocIter != relocEnd; ++relocIter) {
      Relocation* reloc = llvm::cast<Relocation>(relocIter);

      // bypass the relocation with NONE type. This is to avoid overwrite the
      // target result by NONE type relocation if there is a place which has
      // two relocations to apply to, and one of it is NONE type. The result
      // we want is the value of the other relocation result. For example,
      // in .exidx, there are usually an R_ARM_NONE and R_ARM_PREL31 apply to
      // the same place
      if (0x0 == reloc->type())
        continue;
      writeRelocationResult(*reloc, data);
    }
  }

  pOutput.clear();
}

void FragmentLinker::writeRelocationResult(Relocation& pReloc, uint8_t* pOutput)
{
  // get output file offset
  size_t out_offset = pReloc.targetRef().frag()->getParent()->getSection().offset() +
                      pReloc.targetRef().getOutputOffset();

  uint8_t* target_addr = pOutput + out_offset;
  // byte swapping if target and host has different endian, and then write back
  if(llvm::sys::isLittleEndianHost() != m_Backend.isLittleEndian()) {
     uint64_t tmp_data = 0;

     switch(m_Backend.bitclass()) {
       case 32u:
         tmp_data = bswap32(pReloc.target());
         std::memcpy(target_addr, &tmp_data, 4);
         break;

       case 64u:
         tmp_data = bswap64(pReloc.target());
         std::memcpy(target_addr, &tmp_data, 8);
         break;

       default:
         break;
    }
  }
  else
    std::memcpy(target_addr, &pReloc.target(), m_Backend.bitclass()/8);
}

/// isOutputPIC - return whether the output is position-independent
bool FragmentLinker::isOutputPIC() const
{
  return checkIsOutputPIC();
}

/// isStaticLink - return whether we're doing static link
bool FragmentLinker::isStaticLink() const
{
  return checkIsStaticLink();
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
  if (m_Module.getLibraryList().empty() && !isOutputPIC())
    return true;
  return false;
}

