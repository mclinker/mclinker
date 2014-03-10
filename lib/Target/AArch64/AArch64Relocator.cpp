//===- AArch64Relocator.cpp  ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/LinkerConfig.h>
#include <mcld/IRBuilder.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/ELFFileFormat.h>
#include <mcld/Object/ObjectBuilder.h>

#include <llvm/ADT/Twine.h>
#include <llvm/Support/DataTypes.h>
#include <llvm/Support/ELF.h>
#include <llvm/Support/Host.h>

#include "AArch64Relocator.h"
#include "AArch64RelocationFunctions.h"

using namespace mcld;

//===----------------------------------------------------------------------===//
// Relocation helper functions
//===----------------------------------------------------------------------===//
// Return true if overflow
static bool
helper_check_signed_overflow(AArch64Relocator::DWord pValue, unsigned bits)
{
  if (bits >= sizeof(int64_t) * 8)
    return false;
  int64_t signed_val = static_cast<int64_t>(pValue);
  int64_t max = (1 << (bits - 1)) - 1;
  int64_t min = -(1 << (bits - 1));
  if (signed_val > max || signed_val < min)
    return true;
  return false;
}

static Relocator::Address helper_get_page_address(Relocator::Address pValue)
{
  return (pValue & ~ (Relocator::Address) 0xFFF);
}

static Relocator::Address helper_get_page_offset(Relocator::Address pValue)
{
  return (pValue & (Relocator::Address) 0xFFF);
}

static inline uint32_t get_mask(uint32_t pValue)
{
  return ((1u << (pValue)) - 1);
}

static uint32_t
helper_reencode_adr_imm(uint32_t insn, uint32_t imm)
{
  return (insn & ~((get_mask(2) << 29) | (get_mask(19) << 5)))
     | ((imm & get_mask(2)) << 29) | ((imm & (get_mask(19) << 2)) << 3);
}

// Reencode the imm field of add immediate.
static inline uint32_t reencode_add_imm(uint32_t pInst, uint32_t pImm)
{
  return (pInst & ~(get_mask(12) << 10)) | ((pImm & get_mask(12)) << 10);
}

static uint32_t
helper_get_upper32(AArch64Relocator::DWord pData)
{
  if (llvm::sys::IsLittleEndianHost)
    return pData >> 32;
  return pData & 0xFFFFFFFF;
}

static void
helper_put_upper32(uint32_t pData, AArch64Relocator::DWord& pDes)
{
  *(reinterpret_cast<uint32_t*>(&pDes)) = pData;
}

static
AArch64Relocator::Address helper_get_PLT_address(ResolveInfo& pSym,
                                             AArch64Relocator& pParent)
{
  PLTEntryBase* plt_entry = pParent.getSymPLTMap().lookUp(pSym);
  assert(NULL != plt_entry);
  return pParent.getTarget().getPLT().addr() + plt_entry->getOffset();
}

static
AArch64PLT1& helper_PLT_init(Relocation& pReloc, AArch64Relocator& pParent)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  AArch64GNULDBackend& ld_backend = pParent.getTarget();
  assert(NULL == pParent.getSymPLTMap().lookUp(*rsym));

  AArch64PLT1* plt_entry = ld_backend.getPLT().create();
  pParent.getSymPLTMap().record(*rsym, *plt_entry);

  // initialize plt and the corresponding gotplt and dyn rel entry.
  assert(NULL == pParent.getSymGOTPLTMap().lookUp(*rsym) &&
         "PLT entry not exist, but DynRel entry exist!");
  AArch64GOTEntry* gotplt_entry = ld_backend.getGOTPLT().createGOTPLT();
  pParent.getSymGOTPLTMap().record(*rsym, *gotplt_entry);

  // init the corresponding rel entry in .rela.plt
  Relocation& rel_entry = *ld_backend.getRelaPLT().create();
  rel_entry.setType(R_AARCH64_JUMP_SLOT);
  rel_entry.targetRef().assign(*gotplt_entry);
  rel_entry.setSymInfo(rsym);
  return *plt_entry;
}

//===----------------------------------------------------------------------===//
// Relocation Functions and Tables
//===----------------------------------------------------------------------===//
DECL_AARCH64_APPLY_RELOC_FUNCS

/// the prototype of applying function
typedef Relocator::Result (*ApplyFunctionType)(Relocation& pReloc,
                                               AArch64Relocator& pParent);

// the table entry of applying functions
class ApplyFunctionEntry {
public:
  ApplyFunctionEntry() {}
  ApplyFunctionEntry(ApplyFunctionType pFunc,
                     const char* pName,
                     size_t pSize = 0)
      : func(pFunc), name(pName), size(pSize) { }
  ApplyFunctionType func;
  const char* name;
  size_t size;
};
typedef std::map<Relocator::Type, ApplyFunctionEntry> ApplyFunctionMap;

static const ApplyFunctionMap::value_type ApplyFunctionList[] = {
  DECL_AARCH64_APPLY_RELOC_FUNC_PTRS(ApplyFunctionMap::value_type,
                                     ApplyFunctionEntry)
};

// declare the table of applying functions
static ApplyFunctionMap ApplyFunctions(ApplyFunctionList,
    ApplyFunctionList + sizeof(ApplyFunctionList)/sizeof(ApplyFunctionList[0]));

//===----------------------------------------------------------------------===//
// AArch64Relocator
//===----------------------------------------------------------------------===//
AArch64Relocator::AArch64Relocator(AArch64GNULDBackend& pParent,
                                   const LinkerConfig& pConfig)
  : Relocator(pConfig),
    m_Target(pParent) {
}

AArch64Relocator::~AArch64Relocator()
{
}

Relocator::Result AArch64Relocator::applyRelocation(Relocation& pRelocation)
{
  Relocation::Type type = pRelocation.type();
  // valid types are 0x0, 0x100-0x239
  if ((type < 0x100 || type > 0x239) && (type != 0x0)) {
    return Relocator::Unknown;
  }
  assert(ApplyFunctions.find(type) != ApplyFunctions.end());
  return ApplyFunctions[type].func(pRelocation, *this);
}

const char* AArch64Relocator::getName(Relocator::Type pType) const
{
  assert(ApplyFunctions.find(pType) != ApplyFunctions.end());
  return ApplyFunctions[pType].name;
}

Relocator::Size AArch64Relocator::getSize(Relocation::Type pType) const
{
  return 64;
}

void AArch64Relocator::addCopyReloc(ResolveInfo& pSym)
{
  Relocation& rel_entry = *getTarget().getRelaDyn().create();
  rel_entry.setType(R_AARCH64_COPY);
  assert(pSym.outSymbol()->hasFragRef());
  rel_entry.targetRef().assign(*pSym.outSymbol()->fragRef());
  rel_entry.setSymInfo(&pSym);
}

/// defineSymbolForCopyReloc
/// For a symbol needing copy relocation, define a copy symbol in the BSS
/// section and all other reference to this symbol should refer to this
/// copy.
/// This is executed at scan relocation stage.
LDSymbol& AArch64Relocator::defineSymbolforCopyReloc(IRBuilder& pBuilder,
                                                     const ResolveInfo& pSym)
{
  // get or create corresponding BSS LDSection
  LDSection* bss_sect_hdr = NULL;
  ELFFileFormat* file_format = getTarget().getOutputFormat();
  if (ResolveInfo::ThreadLocal == pSym.type())
    bss_sect_hdr = &file_format->getTBSS();
  else
    bss_sect_hdr = &file_format->getBSS();

  // get or create corresponding BSS SectionData
  SectionData* bss_data = NULL;
  if (bss_sect_hdr->hasSectionData())
    bss_data = bss_sect_hdr->getSectionData();
  else
    bss_data = IRBuilder::CreateSectionData(*bss_sect_hdr);

  // Determine the alignment by the symbol value
  // FIXME: here we use the largest alignment
  uint32_t addralign = config().targets().bitclass() / 8;

  // allocate space in BSS for the copy symbol
  Fragment* frag = new FillFragment(0x0, 1, pSym.size());
  uint64_t size = ObjectBuilder::AppendFragment(*frag,
                                                *bss_data,
                                                addralign);
  bss_sect_hdr->setSize(bss_sect_hdr->size() + size);

  // change symbol binding to Global if it's a weak symbol
  ResolveInfo::Binding binding = (ResolveInfo::Binding)pSym.binding();
  if (binding == ResolveInfo::Weak)
    binding = ResolveInfo::Global;

  // Define the copy symbol in the bss section and resolve it
  LDSymbol* cpy_sym = pBuilder.AddSymbol<IRBuilder::Force, IRBuilder::Resolve>(
                          pSym.name(),
                          (ResolveInfo::Type)pSym.type(),
                          ResolveInfo::Define,
                          binding,
                          pSym.size(),  // size
                          0x0,          // value
                          FragmentRef::Create(*frag, 0x0),
                          (ResolveInfo::Visibility)pSym.other());

  return *cpy_sym;
}

void
AArch64Relocator::scanLocalReloc(Relocation& pReloc, const LDSection& pSection)
{
}

void AArch64Relocator::scanGlobalReloc(Relocation& pReloc,
                                       IRBuilder& pBuilder,
                                       const LDSection& pSection)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  switch(pReloc.type()) {
    case llvm::ELF::R_AARCH64_PREL64:
    case llvm::ELF::R_AARCH64_PREL32:
    case llvm::ELF::R_AARCH64_PREL16:
      if (getTarget().symbolNeedsPLT(*rsym) &&
          LinkerConfig::DynObj != config().codeGenType()) {
        // create plt for this symbol if it does not have one
        if (!(rsym->reserved() & ReservePLT)){
          // Symbol needs PLT entry, we need a PLT entry
          // and the corresponding GOT and dynamic relocation entry
          // in .got and .rel.plt.
          helper_PLT_init(pReloc, *this);
          // set PLT bit
          rsym->setReserved(rsym->reserved() | ReservePLT);
        }
      }

      // Only PC relative relocation against dynamic symbol needs a
      // dynamic relocation.  Only dynamic copy relocation is allowed
      // and PC relative relocation will be resolved to the local copy.
      // All other dynamic relocations may lead to run-time relocation
      // overflow.
      if (getTarget().isDynamicSymbol(*rsym) &&
	        getTarget().symbolNeedsDynRel(*rsym,
                                        (rsym->reserved() & ReservePLT),
                                        false) &&
          getTarget().symbolNeedsCopyReloc(pReloc, *rsym)) {
   	    LDSymbol& cpy_sym = defineSymbolforCopyReloc(pBuilder, *rsym);
        addCopyReloc(*cpy_sym.resolveInfo());
      }
      return;

    case llvm::ELF::R_AARCH64_JUMP26:
    case llvm::ELF::R_AARCH64_CALL26: {
      // return if we already create plt for this symbol
      if (rsym->reserved() & ReservePLT)
        return;

      // if the symbol's value can be decided at link time, then no need plt
      if (getTarget().symbolFinalValueIsKnown(*rsym))
        return;

      // if symbol is defined in the ouput file and it's not
      // preemptible, no need plt
      if (rsym->isDefine() && !rsym->isDyn() &&
          !getTarget().isSymbolPreemptible(*rsym)) {
        return;
      }

      // Symbol needs PLT entry, we need to reserve a PLT entry
      // and the corresponding GOT and dynamic relocation entry
      // in .got and .rel.plt.
      helper_PLT_init(pReloc, *this);
      // set PLT bit
      rsym->setReserved(rsym->reserved() | ReservePLT);
      return;
    }

    case llvm::ELF::R_AARCH64_ADR_PREL_PG_HI21:
    case R_AARCH64_ADR_PREL_PG_HI21_NC:
      if (getTarget().symbolNeedsDynRel(*rsym,
                                        (rsym->reserved() & ReservePLT),
                                        false)) {
        if (getTarget().symbolNeedsCopyReloc(pReloc, *rsym)) {
   	      LDSymbol& cpy_sym = defineSymbolforCopyReloc(pBuilder, *rsym);
	        addCopyReloc(*cpy_sym.resolveInfo());
        }
      }
      if (getTarget().symbolNeedsPLT(*rsym)) {
        // create plt for this symbol if it does not have one
        if (!(rsym->reserved() & ReservePLT)){
          // Symbol needs PLT entry, we need a PLT entry
          // and the corresponding GOT and dynamic relocation entry
          // in .got and .rel.plt.
          helper_PLT_init(pReloc, *this);
          // set PLT bit
          rsym->setReserved(rsym->reserved() | ReservePLT);
        }
      }
      return;

    default:
      break;
  }
}

void AArch64Relocator::scanRelocation(Relocation& pReloc,
                                      IRBuilder& pBuilder,
                                      Module& pModule,
                                      LDSection& pSection,
                                      Input& pInput)
{
  ResolveInfo* rsym = pReloc.symInfo();
  assert(NULL != rsym &&
         "ResolveInfo of relocation not set while scanRelocation");

  assert(NULL != pSection.getLink());
  if (0 == (pSection.getLink()->flag() & llvm::ELF::SHF_ALLOC))
    return;

  // Scan relocation type to determine if an GOT/PLT/Dynamic Relocation
  // entries should be created.
  // FIXME: Below judgements concern nothing about TLS related relocation

  // rsym is local
  if (rsym->isLocal())
    scanLocalReloc(pReloc, pSection);
  // rsym is external
  else
    scanGlobalReloc(pReloc, pBuilder, pSection);

  // check if we shoule issue undefined reference for the relocation target
  // symbol
  if (rsym->isUndef() && !rsym->isDyn() && !rsym->isWeak() && !rsym->isNull())
    issueUndefRef(pReloc, pSection, pInput);
}

//===----------------------------------------------------------------------===//
// Each relocation function implementation
//===----------------------------------------------------------------------===//

// R_AARCH64_NONE
Relocator::Result none(Relocation& pReloc, AArch64Relocator& pParent)
{
  return Relocator::OK;
}

Relocator::Result unsupport(Relocation& pReloc, AArch64Relocator& pParent)
{
  return Relocator::Unsupport;
}

// R_AARCH64_PREL64: S + A - P
// R_AARCH64_PREL32: S + A - P
// R_AARCH64_PREL16: S + A - P
Relocator::Result rel(Relocation& pReloc, AArch64Relocator& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  AArch64Relocator::Address S = pReloc.symValue();
  AArch64Relocator::DWord   A = pReloc.addend();
  Relocator::DWord P = pReloc.place();

  if (llvm::ELF::R_AARCH64_PREL64 != pReloc.type())
    P = P & get_mask(pParent.getSize(pReloc.type()));

  LDSection& target_sect = pReloc.targetRef().frag()->getParent()->getSection();
  // If the flag of target section is not ALLOC, we will not scan this
  // relocation but perform static relocation. (e.g., applying .debug section)
  if (0x0 != (llvm::ELF::SHF_ALLOC & target_sect.flag())) {
    // if plt entry exists, the S value is the plt entry address
    if (!rsym->isLocal()) {
      if (rsym->reserved() & AArch64Relocator::ReservePLT) {
        S = helper_get_PLT_address(*rsym, pParent);
      }
    }
  }

  pReloc.target() = S + A - P;

  if (llvm::ELF::R_AARCH64_PREL64 != pReloc.type() &&
      helper_check_signed_overflow(pReloc.target(),
                                   pParent.getSize(pReloc.type())))
    return AArch64Relocator::Overflow;
  return AArch64Relocator::OK;
}

// R_AARCH64_ADD_ABS_LO12_NC
Relocator::Result add_abs_lo12(Relocation& pReloc, AArch64Relocator& pParent)
{
  AArch64Relocator::Address value = 0x0;
  AArch64Relocator::Address S = pReloc.symValue();
  AArch64Relocator::DWord   A = pReloc.target() + pReloc.addend();

  value = helper_get_page_offset(S + A);
  pReloc.target() = reencode_add_imm(pReloc.target(), value);

  return Relocator::OK;
}

// R_AARCH64_ADR_PREL_PG_HI21: ((PG(S + A) - PG(P)) >> 12) & 0x1fffff
// R_AARCH64_ADR_PREL_PG_HI21_NC: ((PG(S + A) - PG(P)) >> 12) & 0x1fffff
AArch64Relocator::Result adr_prel_pg_hi21(Relocation& pReloc,
                                          AArch64Relocator& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  AArch64Relocator::Address S = pReloc.symValue();
  // if plt entry exists, the S value is the plt entry address
  if (rsym->reserved() & AArch64Relocator::ReservePLT) {
    S = helper_get_PLT_address(*rsym, pParent);
  }
  AArch64Relocator::DWord   A = pReloc.addend();
  AArch64Relocator::DWord   P = pReloc.place() ;
  AArch64Relocator::DWord   X = helper_get_page_address(S + A) -
                                helper_get_page_address(P);

  // get 32 bit
  AArch64Relocator::DWord content = helper_get_upper32(pReloc.target());
  X >>= 12;
  content = helper_reencode_adr_imm(content, X);
  // put the content back
  helper_put_upper32(content, pReloc.target());

  return AArch64Relocator::OK;
}

