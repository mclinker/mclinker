//===- ELFDynObjReader.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/ADT/Twine.h>
#include <llvm/Support/ErrorHandling.h>
#include <mcld/LD/ELFDynObjReader.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/MC/MCLDInput.h>

#include <string>

using namespace mcld;

//==========================
// ELFDynObjReader
ELFDynObjReader::ELFDynObjReader(TargetLDBackend& pBackend, MCLinker& pLinker)
  : DynObjReader(pBackend), m_Linker(pLinker) {
}

ELFDynObjReader::~ELFDynObjReader()
{
}

LDReader::Endian ELFDynObjReader::endian(Input& pFile) const
{
  // TODO
  return LDReader::LittleEndian;
}

bool ELFDynObjReader::isMyFormat(Input &pFile) const
{
  // TODO: Since normalize use this function before read symbol,
  //       so we check extension first.
  // NOTE: Add ELFObject Cache, check Type and Machine
  return pFile.path().extension().native().find(".so") != std::string::npos;
}

llvm::error_code ELFDynObjReader::readDSO(Input& pFile)
{
  // TODO
  return llvm::error_code();
}

bool ELFDynObjReader::readSymbols(Input& pInput)
{
  std::auto_ptr<ELFObject<32> > rs_object =
    ELFReader::createELFObject(pInput);

  ELFSectionSymTab<32>* rs_symtab =
    static_cast<ELFSectionSymTab<32> *>(rs_object->getSectionByName(".dynsym"));

  // Skip Start NULL Symbol
  size_t rs_sym_idx = 1;

  // for all symbol, add into MCLinker
  for (; rs_sym_idx < rs_symtab->size() ; ++rs_sym_idx) {
    ELFSymbol<32>* rs_sym = (*rs_symtab)[rs_sym_idx];

    /// convert to name
    llvm::StringRef ld_name(rs_sym->getName());

    /// convert to ResolveInfo::Binding
    ResolveInfo::Binding ld_binding = ELFReader::getBindingResolveInfo(rs_sym, false);

    if (ResolveInfo::Local == ld_binding) {
      llvm::report_fatal_error(llvm::Twine("read a local symbol `") +
                               ld_name +
                               llvm::Twine("' in the shared object `") +
                               pInput.path().native() +
                               llvm::Twine("'.\n"));
    }

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
        llvm::errs() << "WARNING: do not support symbol type `"
                     << ld_type
                     << "' of symbol `"
                     << ld_name.str()
                     << ".\n";
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
                                   llvm::Twine("] is invalid.\n"));

        ld_frag_ref = m_Linker.getLayout().getFragmentRef(*sect_hdr,
                                                         rs_sym->getValue());
        break;
      }
    }
    /// convert to ResolveInfo::Visibility
    ResolveInfo::Visibility ld_vis = ELFReader::getVisibilityResolveInfo(rs_sym);


    m_Linker.addGlobalSymbol(ld_name,
                             false,
                             ld_type,
                             ld_desc,
                             ld_binding,
                             ld_size,
                             ld_frag_ref,
                             ld_vis);
  } // end of for

  return true;
}

