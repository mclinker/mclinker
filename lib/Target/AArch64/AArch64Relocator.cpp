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
#include <llvm/ADT/Twine.h>
#include <llvm/Support/DataTypes.h>
#include <llvm/Support/ELF.h>
#include <llvm/Support/Host.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/ELFFileFormat.h>
#include <mcld/Object/ObjectBuilder.h>

#include "AArch64Relocator.h"
#include "AArch64RelocationFunctions.h"

using namespace mcld;

//=========================================//
// Relocation helper function              //
//=========================================//
// Return true if overflow
static bool
helper_check_signed_overflow(AArch64Relocator::DWord pValue,
                             unsigned bits)
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

//===--------------------------------------------------------------------===//
// Relocation Functions and Tables
//===--------------------------------------------------------------------===//
DECL_AARCH64_APPLY_RELOC_FUNCS

/// the prototype of applying function
typedef Relocator::Result (*ApplyFunctionType)(Relocation& pReloc,
                                               AArch64Relocator& pParent);

// the table entry of applying functions
struct ApplyFunctionTriple
{
  ApplyFunctionType func;
  unsigned int type;
  const char* name;
};

// declare the table of applying functions
static const ApplyFunctionTriple ApplyFunctions[] = {
  DECL_AARCH64_APPLY_RELOC_FUNC_PTRS
};

//===--------------------------------------------------------------------===//
// AArch64Relocator
//===--------------------------------------------------------------------===//
AArch64Relocator::AArch64Relocator(AArch64GNULDBackend& pParent,
                                   const LinkerConfig& pConfig)
  : Relocator(pConfig),
    m_Target(pParent) {
}

AArch64Relocator::~AArch64Relocator()
{
}

Relocator::Result
AArch64Relocator::applyRelocation(Relocation& pRelocation)
{
  Relocation::Type type = pRelocation.type();
  // valid types are 0x0, 0x100-0x239
  if ((type < 0x100 || type > 0x239) && (type != 0x0)) {
    return Relocator::Unknown;
  }

  return ApplyFunctions[type].func(pRelocation, *this);
}

const char* AArch64Relocator::getName(Relocator::Type pType) const
{
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
LDSymbol&
AArch64Relocator::defineSymbolforCopyReloc(IRBuilder& pBuilder,
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
  ;LDSymbol* cpy_sym = pBuilder.AddSymbol<IRBuilder::Force, IRBuilder::Resolve>(
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
  switch(pReloc.type()) {
    case llvm::ELF::R_AARCH64_PREL32:
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
  // rsym - The relocation target symbol
  switch(pReloc.type()) {
    case llvm::ELF::R_AARCH64_PREL32:
      return;
    default:
      break;
  }
}

//=========================================//
// Each relocation function implementation //
//=========================================//

// R_AARCH64_NONE
AArch64Relocator::Result none(Relocation& pReloc, AArch64Relocator& pParent)
{
  return AArch64Relocator::OK;
}

AArch64Relocator::Result unsupport(Relocation& pReloc,
                                   AArch64Relocator& pParent)
{
  return AArch64Relocator::Unsupport;
}

// R_AARCH64_PREL32: S + A - P
AArch64Relocator::Result rel(Relocation& pReloc,
                             AArch64Relocator& pParent)
{
  AArch64Relocator::Address S = pReloc.symValue();
  AArch64Relocator::DWord   A = pReloc.target() + pReloc.addend();
  pReloc.target() = S + A - pReloc.place();

  helper_check_signed_overflow(pReloc.target(), 32);
  return AArch64Relocator::OK;
}

