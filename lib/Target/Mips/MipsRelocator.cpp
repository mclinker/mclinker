//===- MipsRelocator.cpp  -----------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "MipsRelocator.h"
#include "MipsRelocationFunctions.h"

#include <mcld/IRBuilder.h>
#include <mcld/LinkerConfig.h>
#include <mcld/Object/ObjectBuilder.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Target/OutputRelocSection.h>
#include <mcld/LD/ELFFileFormat.h>

#include <llvm/ADT/Twine.h>
#include <llvm/Support/ELF.h>

namespace llvm {
namespace ELF {

// FIXME: Consider upstream these relocation types to LLVM.
enum {
  R_MIPS_LA25_LUI = 200,
  R_MIPS_LA25_J   = 201,
  R_MIPS_LA25_ADD = 202,
};

} // end namespace ELF
} // end namespace llvm

using namespace mcld;

//===----------------------------------------------------------------------===//
// MipsRelocationInfo
//===----------------------------------------------------------------------===//
class mcld::MipsRelocationInfo
{
public:
  static bool HasSubType(const Relocation& pParent, Relocation::Type pType)
  {
    if (llvm::ELF::R_MIPS_NONE == pType)
      return true;

    for (Relocation::Type type = pParent.type();
         llvm::ELF::R_MIPS_NONE != (type & 0xff); type >>= 8) {
      if ((type & 0xff) == pType)
        return true;
    }

    return false;
  }

  MipsRelocationInfo(Relocation& pParent, bool pIsRel)
    : m_Parent(&pParent),
      m_Type(pParent.type()),
      m_Addend(0),
      m_Symbol(pParent.symValue()),
      m_Result(pParent.target())
  {
    if (pIsRel && (type() < llvm::ELF::R_MIPS_LA25_LUI ||
                   type() > llvm::ELF::R_MIPS_LA25_ADD))
      m_Addend = pParent.target();
    else
      m_Addend = pParent.addend();
  }

  bool isNone() const
  {
    return llvm::ELF::R_MIPS_NONE == type();
  }

  bool isLast() const
  {
    return llvm::ELF::R_MIPS_NONE == (m_Type >> 8);
  }

  MipsRelocationInfo next() const
  {
    return MipsRelocationInfo(*m_Parent, m_Type >> 8, result(), result(), 0);
  }

  const Relocation& parent() const
  {
    return *m_Parent;
  }

  Relocation& parent()
  {
    return *m_Parent;
  }

  Relocation::Type type() const
  {
    return m_Type & 0xff;
  }

  Relocation::DWord A() const
  {
    return m_Addend;
  }

  Relocation::DWord S() const
  {
    return m_Symbol;
  }

  Relocation::DWord P() const
  {
    return parent().place();
  }

  Relocation::DWord result() const
  {
    return m_Result;
  }

  Relocation::DWord& result()
  {
    return m_Result;
  }

private:
  Relocation* m_Parent;
  Relocation::Type m_Type;
  Relocation::DWord m_Addend;
  Relocation::DWord m_Symbol;
  Relocation::DWord m_Result;

  MipsRelocationInfo(Relocation& pParent, Relocation::Type pType,
                     Relocation::DWord pResult,
                     Relocation::DWord pAddend, Relocation::DWord pSymbol)
    : m_Parent(&pParent),
      m_Type(pType),
      m_Addend(pAddend),
      m_Symbol(pSymbol),
      m_Result(pResult)
  {}

  bool isFirst() const {
    return m_Type == parent().type();
  }
};

//===----------------------------------------------------------------------===//
// Relocation Functions and Tables
//===----------------------------------------------------------------------===//
DECL_MIPS_APPLY_RELOC_FUNCS

/// the prototype of applying function
typedef Relocator::Result (*ApplyFunctionType)(MipsRelocationInfo&,
                                               MipsRelocator& pParent);


// the table entry of applying functions
struct ApplyFunctionTriple
{
  ApplyFunctionType func;
  unsigned int type;
  const char* name;
  unsigned int size;
};

// declare the table of applying functions
static const ApplyFunctionTriple ApplyFunctions[] = {
  DECL_MIPS_APPLY_RELOC_FUNC_PTRS
};

//===----------------------------------------------------------------------===//
// MipsRelocator
//===----------------------------------------------------------------------===//
MipsRelocator::MipsRelocator(MipsGNULDBackend& pParent,
                             const LinkerConfig& pConfig)
  : Relocator(pConfig),
    m_Target(pParent),
    m_pApplyingInput(NULL),
    m_CurrentLo16Reloc(NULL)
{
}

Relocator::Result
MipsRelocator::applyRelocation(Relocation& pReloc)
{
  // If m_CurrentLo16Reloc is not NULL we are processing
  // postponed relocation. Otherwise check relocation type
  // and postpone it for later handling.
  if (NULL == m_CurrentLo16Reloc && isPostponed(pReloc)) {
    postponeRelocation(pReloc);
    return OK;
  }

  for (MipsRelocationInfo info(pReloc, isRel());
       !info.isNone(); info = info.next()) {
    if (info.type() >= sizeof(ApplyFunctions) / sizeof(ApplyFunctions[0]))
      return Unknown;

    const ApplyFunctionTriple & triple = ApplyFunctions[info.type()];

    Result res = triple.func(info, *this);
    if (OK != res)
      return res;

    if (info.isLast()) {
      uint64_t mask = 0xFFFFFFFFFFFFFFFFULL >> (64 - triple.size);
      pReloc.target() &= ~mask;
      pReloc.target() |= info.result() & mask;
    }
  }

  return OK;
}

const char* MipsRelocator::getName(Relocation::Type pType) const
{
  return ApplyFunctions[pType & 0xff].name;
}

Relocator::Size MipsRelocator::getSize(Relocation::Type pType) const
{
  return ApplyFunctions[pType & 0xff].size;
}

void MipsRelocator::scanRelocation(Relocation& pReloc,
                                   IRBuilder& pBuilder,
                                   Module& pModule,
                                   LDSection& pSection,
                                   Input& pInput)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  assert(NULL != rsym && "ResolveInfo of relocation not set while scanRelocation");

  // Skip relocation against _gp_disp
  if (NULL != getTarget().getGpDispSymbol() &&
      rsym == getTarget().getGpDispSymbol()->resolveInfo())
    return;

  assert(NULL != pSection.getLink());
  if (0 == (pSection.getLink()->flag() & llvm::ELF::SHF_ALLOC))
    return;

  for (MipsRelocationInfo info(pReloc, isRel());
       !info.isNone(); info = info.next()) {
    // We test isLocal or if pInputSym is not a dynamic symbol
    // We assume -Bsymbolic to bind all symbols internaly via !rsym->isDyn()
    // Don't put undef symbols into local entries.
    if (isLocalReloc(*rsym))
      scanLocalReloc(info, pBuilder, pSection);
    else
      scanGlobalReloc(info, pBuilder, pSection);

    if (getTarget().needsLA25Stub(info.type(), info.parent().symInfo()))
      getTarget().addNonPICBranchSym(pReloc.symInfo());
  }

  // Check if we should issue undefined reference
  // for the relocation target symbol.
  if (rsym->isUndef() && !rsym->isDyn() && !rsym->isWeak() && !rsym->isNull())
    issueUndefRef(pReloc, pSection, pInput);
}

bool MipsRelocator::initializeScan(Input& pInput)
{
  if (LinkerConfig::Object != config().codeGenType())
    getTarget().getGOT().initializeScan(pInput);
  return true;
}

bool MipsRelocator::finalizeScan(Input& pInput)
{
  if (LinkerConfig::Object != config().codeGenType())
    getTarget().getGOT().finalizeScan(pInput);
  return true;
}

bool MipsRelocator::initializeApply(Input& pInput)
{
  m_pApplyingInput = &pInput;
  return true;
}

bool MipsRelocator::finalizeApply(Input& pInput)
{
  m_pApplyingInput = NULL;
  return true;
}

void MipsRelocator::scanLocalReloc(MipsRelocationInfo& pReloc,
                                   IRBuilder& pBuilder,
                                   const LDSection& pSection)
{
  ResolveInfo* rsym = pReloc.parent().symInfo();

  switch (pReloc.type()){
    case llvm::ELF::R_MIPS_NONE:
    case llvm::ELF::R_MIPS_16:
      break;
    case llvm::ELF::R_MIPS_32:
    case llvm::ELF::R_MIPS_64:
      if (LinkerConfig::DynObj == config().codeGenType()) {
        // TODO: (simon) The gold linker does not create an entry in .rel.dyn
        // section if the symbol section flags contains SHF_EXECINSTR.
        // 1. Find the reason of this condition.
        // 2. Check this condition here.
        getTarget().getRelDyn().reserveEntry();
        rsym->setReserved(rsym->reserved() | ReserveRel);
        getTarget().checkAndSetHasTextRel(*pSection.getLink());
      }
      break;
    case llvm::ELF::R_MIPS_REL32:
    case llvm::ELF::R_MIPS_26:
    case llvm::ELF::R_MIPS_HI16:
    case llvm::ELF::R_MIPS_LO16:
    case llvm::ELF::R_MIPS_PC16:
    case llvm::ELF::R_MIPS_SHIFT5:
    case llvm::ELF::R_MIPS_SHIFT6:
    case llvm::ELF::R_MIPS_SUB:
    case llvm::ELF::R_MIPS_INSERT_A:
    case llvm::ELF::R_MIPS_INSERT_B:
    case llvm::ELF::R_MIPS_DELETE:
    case llvm::ELF::R_MIPS_HIGHER:
    case llvm::ELF::R_MIPS_HIGHEST:
    case llvm::ELF::R_MIPS_SCN_DISP:
    case llvm::ELF::R_MIPS_REL16:
    case llvm::ELF::R_MIPS_ADD_IMMEDIATE:
    case llvm::ELF::R_MIPS_PJUMP:
    case llvm::ELF::R_MIPS_RELGOT:
    case llvm::ELF::R_MIPS_JALR:
    case llvm::ELF::R_MIPS_GLOB_DAT:
    case llvm::ELF::R_MIPS_COPY:
    case llvm::ELF::R_MIPS_JUMP_SLOT:
      break;
    case llvm::ELF::R_MIPS_GOT16:
    case llvm::ELF::R_MIPS_CALL16:
    case llvm::ELF::R_MIPS_GOT_HI16:
    case llvm::ELF::R_MIPS_CALL_HI16:
    case llvm::ELF::R_MIPS_GOT_LO16:
    case llvm::ELF::R_MIPS_CALL_LO16:
    case llvm::ELF::R_MIPS_GOT_DISP:
    case llvm::ELF::R_MIPS_GOT_PAGE:
    case llvm::ELF::R_MIPS_GOT_OFST:
      if (getTarget().getGOT().reserveLocalEntry(*rsym,
                                                 pReloc.type(), pReloc.A())) {
        if (getTarget().getGOT().hasMultipleGOT())
          getTarget().checkAndSetHasTextRel(*pSection.getLink());
      }
      break;
    case llvm::ELF::R_MIPS_GPREL32:
    case llvm::ELF::R_MIPS_GPREL16:
    case llvm::ELF::R_MIPS_LITERAL:
      break;
    case llvm::ELF::R_MIPS_TLS_DTPMOD32:
    case llvm::ELF::R_MIPS_TLS_DTPREL32:
    case llvm::ELF::R_MIPS_TLS_DTPMOD64:
    case llvm::ELF::R_MIPS_TLS_DTPREL64:
    case llvm::ELF::R_MIPS_TLS_GD:
    case llvm::ELF::R_MIPS_TLS_LDM:
    case llvm::ELF::R_MIPS_TLS_DTPREL_HI16:
    case llvm::ELF::R_MIPS_TLS_DTPREL_LO16:
    case llvm::ELF::R_MIPS_TLS_GOTTPREL:
    case llvm::ELF::R_MIPS_TLS_TPREL32:
    case llvm::ELF::R_MIPS_TLS_TPREL64:
    case llvm::ELF::R_MIPS_TLS_TPREL_HI16:
    case llvm::ELF::R_MIPS_TLS_TPREL_LO16:
      break;
    case llvm::ELF::R_MIPS_PC32:
      break;
    default:
      fatal(diag::unknown_relocation) << (int)pReloc.type() << rsym->name();
  }
}

void MipsRelocator::scanGlobalReloc(MipsRelocationInfo& pReloc,
                                    IRBuilder& pBuilder,
                                    const LDSection& pSection)
{
  ResolveInfo* rsym = pReloc.parent().symInfo();

  switch (pReloc.type()){
    case llvm::ELF::R_MIPS_NONE:
    case llvm::ELF::R_MIPS_INSERT_A:
    case llvm::ELF::R_MIPS_INSERT_B:
    case llvm::ELF::R_MIPS_DELETE:
    case llvm::ELF::R_MIPS_TLS_DTPMOD64:
    case llvm::ELF::R_MIPS_TLS_DTPREL64:
    case llvm::ELF::R_MIPS_REL16:
    case llvm::ELF::R_MIPS_ADD_IMMEDIATE:
    case llvm::ELF::R_MIPS_PJUMP:
    case llvm::ELF::R_MIPS_RELGOT:
    case llvm::ELF::R_MIPS_TLS_TPREL64:
      break;
    case llvm::ELF::R_MIPS_32:
    case llvm::ELF::R_MIPS_64:
    case llvm::ELF::R_MIPS_HI16:
    case llvm::ELF::R_MIPS_LO16:
      if (getTarget().symbolNeedsDynRel(*rsym, false, true)) {
        getTarget().getRelDyn().reserveEntry();
        if (getTarget().symbolNeedsCopyReloc(pReloc.parent(), *rsym)) {
          LDSymbol& cpySym = defineSymbolforCopyReloc(pBuilder, *rsym);
          addCopyReloc(*cpySym.resolveInfo());
        }
        else {
          // set Rel bit
          rsym->setReserved(rsym->reserved() | ReserveRel);
          getTarget().checkAndSetHasTextRel(*pSection.getLink());
        }
      }
      break;
    case llvm::ELF::R_MIPS_GOT16:
    case llvm::ELF::R_MIPS_CALL16:
    case llvm::ELF::R_MIPS_GOT_DISP:
    case llvm::ELF::R_MIPS_GOT_HI16:
    case llvm::ELF::R_MIPS_CALL_HI16:
    case llvm::ELF::R_MIPS_GOT_LO16:
    case llvm::ELF::R_MIPS_CALL_LO16:
    case llvm::ELF::R_MIPS_GOT_PAGE:
    case llvm::ELF::R_MIPS_GOT_OFST:
      if (getTarget().getGOT().reserveGlobalEntry(*rsym)) {
        if (getTarget().getGOT().hasMultipleGOT())
          getTarget().checkAndSetHasTextRel(*pSection.getLink());
      }
      break;
    case llvm::ELF::R_MIPS_LITERAL:
    case llvm::ELF::R_MIPS_GPREL32:
      fatal(diag::invalid_global_relocation) << (int)pReloc.type()
                                             << rsym->name();
      break;
    case llvm::ELF::R_MIPS_GPREL16:
      break;
    case llvm::ELF::R_MIPS_26:
      // Create a PLT entry if the symbol requires it and does not have it.
      if (getTarget().symbolNeedsPLT(*rsym) &&
          !(rsym->reserved() & ReservePLT)) {
        getTarget().getPLT().reserveEntry();
        getTarget().getGOTPLT().reserve();
        getTarget().getRelPLT().reserveEntry();
        rsym->setReserved(rsym->reserved() | ReservePLT);
      }
      break;
    case llvm::ELF::R_MIPS_PC16:
      break;
    case llvm::ELF::R_MIPS_16:
    case llvm::ELF::R_MIPS_SHIFT5:
    case llvm::ELF::R_MIPS_SHIFT6:
    case llvm::ELF::R_MIPS_SUB:
    case llvm::ELF::R_MIPS_HIGHER:
    case llvm::ELF::R_MIPS_HIGHEST:
    case llvm::ELF::R_MIPS_SCN_DISP:
      break;
    case llvm::ELF::R_MIPS_TLS_DTPREL32:
    case llvm::ELF::R_MIPS_TLS_GD:
    case llvm::ELF::R_MIPS_TLS_LDM:
    case llvm::ELF::R_MIPS_TLS_DTPREL_HI16:
    case llvm::ELF::R_MIPS_TLS_DTPREL_LO16:
    case llvm::ELF::R_MIPS_TLS_GOTTPREL:
    case llvm::ELF::R_MIPS_TLS_TPREL32:
    case llvm::ELF::R_MIPS_TLS_TPREL_HI16:
    case llvm::ELF::R_MIPS_TLS_TPREL_LO16:
      break;
    case llvm::ELF::R_MIPS_REL32:
    case llvm::ELF::R_MIPS_JALR:
    case llvm::ELF::R_MIPS_PC32:
      break;
    case llvm::ELF::R_MIPS_COPY:
    case llvm::ELF::R_MIPS_GLOB_DAT:
    case llvm::ELF::R_MIPS_JUMP_SLOT:
      fatal(diag::dynamic_relocation) << (int)pReloc.type();
      break;
    default:
      fatal(diag::unknown_relocation) << (int)pReloc.type() << rsym->name();
  }
}

bool MipsRelocator::isPostponed(const Relocation& pReloc) const
{
  if (MipsRelocationInfo::HasSubType(pReloc, llvm::ELF::R_MIPS_HI16))
    return true;

  if (MipsRelocationInfo::HasSubType(pReloc, llvm::ELF::R_MIPS_GOT16) &&
      pReloc.symInfo()->isLocal())
    return true;

  return false;
}

void MipsRelocator::addCopyReloc(ResolveInfo& pSym)
{
  Relocation& relEntry = *getTarget().getRelDyn().consumeEntry();
  relEntry.setType(llvm::ELF::R_MIPS_COPY);
  assert(pSym.outSymbol()->hasFragRef());
  relEntry.targetRef().assign(*pSym.outSymbol()->fragRef());
  relEntry.setSymInfo(&pSym);
}

LDSymbol& MipsRelocator::defineSymbolforCopyReloc(IRBuilder& pBuilder,
                                                  const ResolveInfo& pSym)
{
  // Get or create corresponding BSS LDSection
  ELFFileFormat* fileFormat = getTarget().getOutputFormat();
  LDSection* bssSectHdr =
    ResolveInfo::ThreadLocal == pSym.type() ? &fileFormat->getTBSS()
                                            : &fileFormat->getBSS();

  // Get or create corresponding BSS SectionData
  SectionData* bssData =
    bssSectHdr->hasSectionData() ? bssSectHdr->getSectionData()
                                 : IRBuilder::CreateSectionData(*bssSectHdr);

  // Determine the alignment by the symbol value
  // FIXME: here we use the largest alignment
  uint32_t addrAlign = config().targets().bitclass() / 8;

  // Allocate space in BSS for the copy symbol
  Fragment* frag = new FillFragment(0x0, 1, pSym.size());
  uint64_t size = ObjectBuilder::AppendFragment(*frag, *bssData, addrAlign);
  bssSectHdr->setSize(bssSectHdr->size() + size);

  // Change symbol binding to Global if it's a weak symbol
  ResolveInfo::Binding binding = (ResolveInfo::Binding)pSym.binding();
  if (binding == ResolveInfo::Weak)
    binding = ResolveInfo::Global;

  // Define the copy symbol in the bss section and resolve it
  LDSymbol* cpySym = pBuilder.AddSymbol<IRBuilder::Force, IRBuilder::Resolve>(
                      pSym.name(),
                      (ResolveInfo::Type)pSym.type(),
                      ResolveInfo::Define,
                      binding,
                      pSym.size(),  // size
                      0x0,          // value
                      FragmentRef::Create(*frag, 0x0),
                      (ResolveInfo::Visibility)pSym.other());

  // Output all other alias symbols if any
  Module::AliasList* alias_list = pBuilder.getModule().getAliasList(pSym);
  if (NULL == alias_list)
    return *cpySym;

  for (Module::alias_iterator it = alias_list->begin(), ie = alias_list->end();
       it != ie; ++it) {
    const ResolveInfo* alias = *it;
    if (alias == &pSym || !alias->isDyn())
      continue;

    pBuilder.AddSymbol<IRBuilder::Force, IRBuilder::Resolve>(
        alias->name(),
        (ResolveInfo::Type)alias->type(),
        ResolveInfo::Define,
        binding,
        alias->size(),  // size
        0x0,            // value
        FragmentRef::Create(*frag, 0x0),
        (ResolveInfo::Visibility)alias->other());
  }

  return *cpySym;
}

void MipsRelocator::postponeRelocation(Relocation& pReloc)
{
  ResolveInfo* rsym = pReloc.symInfo();
  m_PostponedRelocs[rsym].insert(&pReloc);
}

void MipsRelocator::applyPostponedRelocations(MipsRelocationInfo& pLo16Reloc)
{
  m_CurrentLo16Reloc = &pLo16Reloc;

  ResolveInfo* rsym = pLo16Reloc.parent().symInfo();

  RelocationSet & relocs = m_PostponedRelocs[rsym];
  for (RelocationSet::iterator it = relocs.begin(); it != relocs.end(); ++it)
    (*it)->apply(*this);

  m_PostponedRelocs.erase(rsym);

  m_CurrentLo16Reloc = NULL;
}

bool MipsRelocator::isGpDisp(const Relocation& pReloc) const
{
  return 0 == strcmp("_gp_disp", pReloc.symInfo()->name());
}

bool MipsRelocator::isRel() const
{
  return config().targets().is32Bits();
}

bool MipsRelocator::isLocalReloc(ResolveInfo& pSym) const
{
  if (pSym.isUndef())
    return false;

  return pSym.isLocal() ||
         !getTarget().isDynamicSymbol(pSym) ||
         !pSym.isDyn();
}

Relocator::Address MipsRelocator::getGPAddress()
{
  return getTarget().getGOT().getGPAddr(getApplyingInput());
}

Relocator::Address MipsRelocator::getGP0()
{
  return getTarget().getGP0(getApplyingInput());
}

Fragment& MipsRelocator::getLocalGOTEntry(MipsRelocationInfo& pReloc,
                                          Relocation::DWord entryValue)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.parent().symInfo();
  MipsGOT& got = getTarget().getGOT();

  assert(isLocalReloc(*rsym) &&
         "Attempt to get a global GOT entry for the local relocation");

  Fragment* got_entry = got.lookupLocalEntry(rsym, entryValue);

  // Found a mapping, then return the mapped entry immediately.
  if (NULL != got_entry)
    return *got_entry;

  // Not found.
  got_entry = got.consumeLocal();

  if (got.isPrimaryGOTConsumed())
    setupRelDynEntry(*FragmentRef::Create(*got_entry, 0), NULL);
  else
    got.setEntryValue(got_entry, entryValue);

  got.recordLocalEntry(rsym, entryValue, got_entry);

  return *got_entry;
}

Fragment& MipsRelocator::getGlobalGOTEntry(MipsRelocationInfo& pReloc)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.parent().symInfo();
  MipsGOT& got = getTarget().getGOT();

  assert(!isLocalReloc(*rsym) &&
         "Attempt to get a local GOT entry for the global relocation");

  Fragment* got_entry = got.lookupGlobalEntry(rsym);

  // Found a mapping, then return the mapped entry immediately.
  if (NULL != got_entry)
    return *got_entry;

  // Not found.
  got_entry = got.consumeGlobal();

  if (got.isPrimaryGOTConsumed())
    setupRelDynEntry(*FragmentRef::Create(*got_entry, 0), rsym);
  else
    got.setEntryValue(got_entry, pReloc.parent().symValue());

  got.recordGlobalEntry(rsym, got_entry);

  return *got_entry;
}

Relocator::Address MipsRelocator::getGOTOffset(MipsRelocationInfo& pReloc)
{
  ResolveInfo* rsym = pReloc.parent().symInfo();
  MipsGOT& got = getTarget().getGOT();

  if (isLocalReloc(*rsym)) {
    uint64_t value = pReloc.S();

    if (ResolveInfo::Section == rsym->type())
      value += pReloc.A();

    return got.getGPRelOffset(getApplyingInput(),
                              getLocalGOTEntry(pReloc, value));
  }
  else {
    return got.getGPRelOffset(getApplyingInput(), getGlobalGOTEntry(pReloc));
  }
}

void MipsRelocator::createDynRel(MipsRelocationInfo& pReloc)
{
  Relocator::DWord A = pReloc.A();
  Relocator::DWord S = pReloc.S();

  ResolveInfo* rsym = pReloc.parent().symInfo();

  if (isLocalReloc(*rsym)) {
    setupRelDynEntry(pReloc.parent().targetRef(), NULL);
    pReloc.result() = A + S;
  }
  else {
    setupRelDynEntry(pReloc.parent().targetRef(), rsym);
    // Don't add symbol value that will be resolved by the dynamic linker.
    pReloc.result() = A;
  }
}

uint64_t MipsRelocator::calcAHL(const MipsRelocationInfo& pHiReloc)
{
  assert(NULL != m_CurrentLo16Reloc &&
         "There is no saved R_MIPS_LO16 relocation");

  uint64_t AHI = pHiReloc.A() & 0xFFFF;
  uint64_t ALO = m_CurrentLo16Reloc->A() & 0xFFFF;
  uint64_t AHL = (AHI << 16) + int16_t(ALO);

  return AHL;
}

bool MipsRelocator::isN64ABI() const
{
  return config().targets().is64Bits();
}

uint64_t MipsRelocator::getPLTAddress(ResolveInfo& rsym)
{
  assert((rsym.reserved() & MipsRelocator::ReservePLT) &&
         "Symbol does not require a PLT entry");

  SymPLTMap::const_iterator it = m_SymPLTMap.find(&rsym);

  Fragment* plt;

  if (it != m_SymPLTMap.end()) {
    plt = it->second.first;
  }
  else {
    plt = getTarget().getPLT().consume();

    Fragment* got = getTarget().getGOTPLT().consume();
    Relocation* rel = getTarget().getRelPLT().consumeEntry();

    rel->setType(llvm::ELF::R_MIPS_JUMP_SLOT);
    rel->targetRef().assign(*got);
    rel->setSymInfo(&rsym);

    m_SymPLTMap[&rsym] = PLTDescriptor(plt, got);
  }

  return getTarget().getPLT().addr() + plt->getOffset();
}

//===----------------------------------------------------------------------===//
// Mips32Relocator
//===----------------------------------------------------------------------===//
Mips32Relocator::Mips32Relocator(Mips32GNULDBackend& pParent,
                                 const LinkerConfig& pConfig)
  : MipsRelocator(pParent, pConfig)
{}

void Mips32Relocator::setupRelDynEntry(FragmentRef& pFragRef, ResolveInfo* pSym)
{
  Relocation& relEntry = *getTarget().getRelDyn().consumeEntry();
  relEntry.setType(llvm::ELF::R_MIPS_REL32);
  relEntry.targetRef() = pFragRef;
  relEntry.setSymInfo(pSym);
}

//===----------------------------------------------------------------------===//
// Mips64Relocator
//===----------------------------------------------------------------------===//
Mips64Relocator::Mips64Relocator(Mips64GNULDBackend& pParent,
                                 const LinkerConfig& pConfig)
  : MipsRelocator(pParent, pConfig)
{}

void Mips64Relocator::setupRelDynEntry(FragmentRef& pFragRef, ResolveInfo* pSym)
{
  Relocation::Type type = llvm::ELF::R_MIPS_REL32 |
                          llvm::ELF::R_MIPS_64 << 8;
  // FIXME (simon): Fix dynamic relocations.
  type = llvm::ELF::R_MIPS_NONE;

  Relocation& relEntry = *getTarget().getRelDyn().consumeEntry();
  relEntry.setType(type);
  relEntry.targetRef() = pFragRef;
  relEntry.setSymInfo(pSym);
}

//=========================================//
// Relocation functions implementation     //
//=========================================//

// R_MIPS_NONE and those unsupported/deprecated relocation type
static
MipsRelocator::Result none(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  return Relocator::OK;
}

// R_MIPS_32: S + A
static
MipsRelocator::Result abs32(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  ResolveInfo* rsym = pReloc.parent().symInfo();

  Relocator::DWord A = pReloc.A();
  Relocator::DWord S = pReloc.S();

  LDSection& target_sect =
    pReloc.parent().targetRef().frag()->getParent()->getSection();

  // If the flag of target section is not ALLOC, we will not scan this relocation
  // but perform static relocation. (e.g., applying .debug section)
  if (0x0 == (llvm::ELF::SHF_ALLOC & target_sect.flag())) {
    pReloc.result() = S + A;
    return Relocator::OK;
  }

  if (rsym->reserved() & MipsRelocator::ReserveRel) {
    pParent.createDynRel(pReloc);
    return Relocator::OK;
  }

  pReloc.result() = S + A;

  return Relocator::OK;
}

// R_MIPS_26:
//   local   : ((A | ((P + 4) & 0x3F000000)) + S) >> 2
//   external: (sign–extend(A) + S) >> 2
static
MipsRelocator::Result rel26(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  ResolveInfo* rsym = pReloc.parent().symInfo();

  int32_t A = ((pReloc.parent().target() & 0x03FFFFFF) << 2);
  int32_t P = pReloc.P();
  int32_t S = rsym->reserved() & MipsRelocator::ReservePLT
                  ? pParent.getPLTAddress(*rsym)
                  : pReloc.S();

  if (rsym->isLocal())
    pReloc.result() = A | ((P + 4) & 0x3F000000);
  else
    pReloc.result() = mcld::signExtend<28>(A);

  pReloc.result() = (pReloc.result() + S) >> 2;

  return Relocator::OK;
}

// R_MIPS_HI16:
//   local/external: ((AHL + S) - (short)(AHL + S)) >> 16
//   _gp_disp      : ((AHL + GP - P) - (short)(AHL + GP - P)) >> 16
static
MipsRelocator::Result hi16(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  uint64_t AHL = pParent.calcAHL(pReloc);

  if (pParent.isGpDisp(pReloc.parent())) {
    int32_t P = pReloc.P();
    int32_t GP = pParent.getGPAddress();
    pReloc.result() = ((AHL + GP - P) - (int16_t)(AHL + GP - P)) >> 16;
  }
  else {
    int32_t S = pReloc.S();
    if (pParent.isN64ABI())
      pReloc.result() = (pReloc.A() + S + 0x8000ull) >> 16;
    else
      pReloc.result() = ((AHL + S) - (int16_t)(AHL + S)) >> 16;
  }

  return Relocator::OK;
}

// R_MIPS_LO16:
//   local/external: AHL + S
//   _gp_disp      : AHL + GP - P + 4
static
MipsRelocator::Result lo16(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  // AHL is a combination of HI16 and LO16 addends. But R_MIPS_LO16
  // uses low 16 bits of the AHL. That is why we do not need R_MIPS_HI16
  // addend here.
  int32_t AHL = (pReloc.A() & 0xFFFF);

  if (pParent.isGpDisp(pReloc.parent())) {
    int32_t P = pReloc.P();
    int32_t GP = pParent.getGPAddress();
    pReloc.result() = AHL + GP - P + 4;
  }
  else {
    int32_t S = pReloc.S();
    pReloc.result() = AHL + S;
  }

  pParent.applyPostponedRelocations(pReloc);

  return Relocator::OK;
}

// R_MIPS_GPREL16:
//   external: sign–extend(A) + S - GP
//   local   : sign–extend(A) + S + GP0 – GP
static
MipsRelocator::Result gprel16(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  // Remember to add the section offset to A.
  uint64_t A = pReloc.A();
  uint64_t S = pReloc.S();
  uint64_t GP0 = pParent.getGP0();
  uint64_t GP = pParent.getGPAddress();

  ResolveInfo* rsym = pReloc.parent().symInfo();
  if (rsym->isLocal())
    pReloc.result() = A + S + GP0 - GP;
  else
    pReloc.result() = A + S - GP;

  return Relocator::OK;
}

// R_MIPS_GOT16:
//   local   : G (calculate AHL and put high 16 bit to GOT)
//   external: G
static
MipsRelocator::Result got16(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  if (pReloc.parent().symInfo()->isLocal()) {
    int32_t AHL = pParent.calcAHL(pReloc);
    int32_t S = pReloc.S();
    int32_t res = (AHL + S + 0x8000) & 0xFFFF0000;

    MipsGOT& got = pParent.getTarget().getGOT();

    Fragment& got_entry = pParent.getLocalGOTEntry(pReloc, res);

    pReloc.result() = got.getGPRelOffset(pParent.getApplyingInput(), got_entry);
  }
  else {
    pReloc.result() = pParent.getGOTOffset(pReloc);
  }

  return Relocator::OK;
}

// R_MIPS_GOTHI16:
//   external: (G - (short)G) >> 16 + A
static
MipsRelocator::Result gothi16(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  Relocator::Address G = pParent.getGOTOffset(pReloc);
  int32_t A = pReloc.A();

  pReloc.result() = (G - (int16_t)G) >> (16 + A);

  return Relocator::OK;
}

// R_MIPS_GOTLO16:
//   external: G & 0xffff
static
MipsRelocator::Result gotlo16(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  pReloc.result() = pParent.getGOTOffset(pReloc) & 0xffff;

  return Relocator::OK;
}

// R_MIPS_SUB:
//   external/local: S - A
static
MipsRelocator::Result sub(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  uint64_t S = pReloc.S();
  uint64_t A = pReloc.A();

  pReloc.result() = S - A;

  return Relocator::OK;
}

// R_MIPS_CALL16: G
static
MipsRelocator::Result call16(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  pReloc.result() = pParent.getGOTOffset(pReloc);

  return Relocator::OK;
}

// R_MIPS_GPREL32: A + S + GP0 - GP
static
MipsRelocator::Result gprel32(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  // Remember to add the section offset to A.
  uint64_t A = pReloc.A();
  uint64_t S = pReloc.S();
  uint64_t GP0 = pParent.getGP0();
  uint64_t GP = pParent.getGPAddress();

  pReloc.result() = A + S + GP0 - GP;

  return Relocator::OK;
}

// R_MIPS_64: S + A
static
MipsRelocator::Result abs64(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  // FIXME (simon): Consider to merge with abs32() or use the same function
  // but with another mask size.
  ResolveInfo* rsym = pReloc.parent().symInfo();

  Relocator::DWord A = pReloc.A();
  Relocator::DWord S = pReloc.S();

  LDSection& target_sect =
    pReloc.parent().targetRef().frag()->getParent()->getSection();

  // If the flag of target section is not ALLOC, we will not scan this relocation
  // but perform static relocation. (e.g., applying .debug section)
  if (0x0 == (llvm::ELF::SHF_ALLOC & target_sect.flag())) {
    pReloc.result() = S + A;
    return Relocator::OK;
  }

  if (rsym->reserved() & MipsRelocator::ReserveRel) {
    pParent.createDynRel(pReloc);
    return Relocator::OK;
  }

  pReloc.result() = S + A;

  return Relocator::OK;
}

// R_MIPS_GOT_DISP / R_MIPS_GOT_PAGE: G
static
MipsRelocator::Result gotdisp(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  pReloc.result() = pParent.getGOTOffset(pReloc);

  return Relocator::OK;
}

// R_MIPS_GOT_OFST:
static
MipsRelocator::Result gotoff(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  // FIXME (simon): Needs to be implemented.
  return Relocator::OK;
}

// R_MIPS_JALR:
static
MipsRelocator::Result jalr(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  return Relocator::OK;
}

// R_MIPS_LA25_LUI
static
MipsRelocator::Result la25lui(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  int32_t S = pReloc.S();

  pReloc.result() = (S + 0x8000) >> 16;

  return Relocator::OK;
}

// R_MIPS_LA25_J
static
MipsRelocator::Result la25j(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  int32_t S = pReloc.S();

  pReloc.result() = S >> 2;

  return Relocator::OK;
}

// R_MIPS_LA25_ADD
static
MipsRelocator::Result la25add(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  pReloc.result() = pReloc.S();

  return Relocator::OK;
}

// R_MIPS_PC32:
static
MipsRelocator::Result pc32(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  return Relocator::OK;
}

static
MipsRelocator::Result unsupport(MipsRelocationInfo& pReloc, MipsRelocator& pParent)
{
  return Relocator::Unsupport;
}
