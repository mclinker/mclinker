//===- ELFObjectReader.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/Support/ELF.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/ErrorHandling.h>
#include "mcld/LD/ELFObjectReader.h"
#include "mcld/MC/MCLDInput.h"
#include "mcld/MC/MCLinker.h"
#include "mcld/Target/TargetLDBackend.h"
#include "mcld/MC/MCRegionFragment.h"
#include "mcld/Support/rslinker/ELFSectionHeaderTable.h"
#include "mcld/Support/rslinker/ELFSectionRelTable.h"

#include <string>
#include <cassert>

using namespace llvm;
using namespace mcld;

//==========================
// ELFObjectReader
ELFObjectReader::ELFObjectReader(TargetLDBackend& pBackend, MCLinker& pLinker)
  : ObjectReader(pBackend),
    ELFReader(),
    m_Linker(pLinker)
{
}

ELFObjectReader::~ELFObjectReader()
{
}

bool ELFObjectReader::isMyFormat(Input &pFile) const
{
  return (MCLDFile::Object == ELFReader::fileType(pFile));
}

LDReader::Endian ELFObjectReader::endian(Input &pFile) const
{
  if (ELFReader::isLittleEndian(pFile))
    return LDReader::LittleEndian;
  return LDReader::BigEndian;
}

llvm::error_code ELFObjectReader::readObject(Input& pFile)
{
  // TODO
  return llvm::error_code();
}

bool ELFObjectReader::readSections(Input& pInput)
{
  ELFObject<32> *object = ELFReader::createELF32Object(pInput);
  llvm::OwningPtr<ELFObject<32> > own_ptr(object);

  const ELFSectionHeaderTable<32> *shtab = object->getSectionHeaderTable();

  // handle sections
  for (size_t i=0; i < object->getSectionNumber(); ++i) {
    const ELFSectionHeader<32> *sh = (*shtab)[i];
    LDSection& ldSect = m_Linker.createSectHdr(sh->getName(),
                                               ELFReader::getLDSectionKind(*sh, sh->getName()),
                                               sh->getType(),
                                               sh->getFlags());

    pInput.context()->getSectionTable().push_back(&ldSect);

    switch(ldSect.kind()) {
      // FIXME: support Debug Kind
      case LDFileFormat::Debug:
      /** Fall through **/
      case LDFileFormat::Regular:
      case LDFileFormat::Note:
      case LDFileFormat::Target:
      case LDFileFormat::MetaData: {
        MemoryRegion* region = pInput.memArea()->request(sh->getOffset(), sh->getSize());
        llvm::MCSectionData& mcSectData = m_Linker.getOrCreateSectData(ldSect);
        new MCRegionFragment(*region, &mcSectData);
        break;
      }
      case LDFileFormat::BSS: {
        LDSection& output_bss = m_Linker.getOrCreateOutputSectHdr(
                                               ".bss",
                                               LDFileFormat::BSS,
                                               llvm::ELF::SHT_NOBITS,
                                               llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE);

        llvm::MCSectionData& sectData = m_Linker.getOrCreateSectData(ldSect);
                             /*  value, valsize, size,          SD */
        new llvm::MCFillFragment(0x0,   1,       sh->getSize(), &sectData);
        output_bss.setSize( output_bss.size() + sh->getSize());
        break;
      }
      // ignore
      case LDFileFormat::Null:
      case LDFileFormat::NamePool:
      case LDFileFormat::Relocation:
        continue;
    }
  } // end of for all sections

  return true;
}

/// readSymbols - read symbols into MCLinker from the input relocatable object.
bool ELFObjectReader::readSymbols(Input& pInput)
{
  assert(pInput.hasContext() && "Input file should has context");

  // -----  initialize rslinker  ----- //
  ELFObject<32>* rs_object = ELFReader::createELF32Object(pInput);
  llvm::OwningPtr<ELFObject<32> > own_ptr(rs_object);

  ELFSectionSymTab<32>* rs_symtab =
    static_cast<ELFSectionSymTab<32> *>(rs_object->getSectionByName(".symtab"));

  // Skip Start NULL Symbol
  size_t rs_sym_idx = 1;

  // -----  add symbols ----- //
  // for all rs_symbol, convert it to LDSymbol
  for (; rs_sym_idx < rs_symtab->size() ; ++rs_sym_idx) {
    ELFSymbol<32>* rs_sym = (*rs_symtab)[rs_sym_idx];

    /// convert to name
    llvm::StringRef ld_name(rs_sym->getName(false));

    /// convert to ResolveInfo::Type
    // type in LDSymbol is one-one mapping to ELF_ST_TYPE
    // Only handle NOTYPE, OBJECT and FUNC
    ResolveInfo::Type ld_type;
    switch(ld_type = static_cast<ResolveInfo::Type>(rs_sym->getType())) {
      case llvm::ELF::STT_NOTYPE:
      case llvm::ELF::STT_OBJECT:
      case llvm::ELF::STT_FUNC:
      case llvm::ELF::STT_COMMON: {
        // assigned
        break;
      }
      case llvm::ELF::STT_SECTION:
      case llvm::ELF::STT_FILE:
      default: {
        // FIXME: support these kinds of symbols
        break;
      }
    }

    /// convert to ResolveInfo::Desc
    ResolveInfo::Desc ld_desc;
    switch (rs_sym->getSectionIndex()) {
    case llvm::ELF::SHN_COMMON:
      ld_desc = ResolveInfo::Common;
      break;
    case llvm::ELF::SHN_UNDEF:
      ld_desc = ResolveInfo::Undefined;
      break;
    default:
      // FIXME: handle with indirect symbol
      // ELF weak alias should be handled as indirect symbol.
      ld_desc = ResolveInfo::Define;
      break;
    }

    /// convert to ResolveInfo::Binding
    ResolveInfo::Binding ld_binding = ELFReader::getBindingResolveInfo(rs_sym, false);

    /// convert to ResolveInfo::SizeType
    ResolveInfo::SizeType ld_size = rs_sym->getSize();

    /// get the input fragment.
    MCFragmentRef ld_frag_ref;
    unsigned int sh_shndx;
    switch (sh_shndx = rs_sym->getSectionIndex()) {
      case llvm::ELF::SHN_ABS:
      case llvm::ELF::SHN_COMMON:
      case llvm::ELF::SHN_UNDEF: {
        // do nothing
        break;
      }
      default: {
        LDSection* sect_hdr = pInput.context()->getSection(sh_shndx);
        if (NULL == sect_hdr)
          llvm::report_fatal_error(llvm::Twine("section[") +
                                   llvm::Twine(sh_shndx) +
                                   llvm::Twine("] is invalid in file `") +
                                   pInput.path().native() +
                                   llvm::Twine("'.\n"));

        ld_frag_ref = m_Linker.getLayout().getFragmentRef(*sect_hdr,
                                                         rs_sym->getValue());
        break;
      }
    }
    /// convert to ResolveInfo::Visibility
    ResolveInfo::Visibility ld_vis = ELFReader::getVisibilityResolveInfo(rs_sym);


    if (ResolveInfo::Local == ld_binding) {
      m_Linker.addLocalSymbol(ld_name,
                              ld_type,
                              ld_desc,
                              ld_size,
                              ld_frag_ref,
                              ld_vis);
    }
    else {
      m_Linker.addGlobalSymbol(ld_name,
                               false,
                               ld_type,
                               ld_desc,
                               ld_binding,
                               ld_size,
                               ld_frag_ref,
                               ld_vis);
    }
  } // end of for all rs_symbol

  return true;
}

