//===- ELFDynObjReader.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/ADT/Twine.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/ErrorHandling.h>

#include <mcld/LD/ELFDynObjReader.h>
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/Target/GNULDBackend.h>
#include <mcld/Support/MemoryRegion.h>

#include <string>

using namespace mcld;

//==========================
// ELFDynObjReader
ELFDynObjReader::ELFDynObjReader(GNULDBackend& pBackend, MCLinker& pLinker)
  : DynObjReader(), ELFReader(pBackend), m_Linker(pLinker) {
}

ELFDynObjReader::~ELFDynObjReader()
{
}

bool ELFDynObjReader::isMyFormat(Input &pInput) const
{
  assert(pInput.hasMemArea());

  // Don't warning about the frequently requests.
  // MemoryArea has a list of cache to handle this.
  MemoryRegion* region =
                   pInput.memArea()->request(0, sizeof(llvm::ELF::Elf64_Ehdr));
  uint8_t* data = region->start();
  return (MCLDFile::DynObj == ELFReader::fileType(data));
}

LDReader::Endian ELFDynObjReader::endian(Input& pInput) const
{
  assert(pInput.hasMemArea());

  // Don't warning about the frequently requests.
  // MemoryArea has a list of cache to handle this.
  MemoryRegion* region =
                   pInput.memArea()->request(0, sizeof(llvm::ELF::Elf64_Ehdr));
  uint8_t* data = region->start();
  if (ELFReader::isLittleEndian(data))
    return LDReader::LittleEndian;
  return LDReader::BigEndian;
}

llvm::error_code ELFDynObjReader::readDSO(Input& pFile)
{
  // TODO
  return llvm::error_code();
}

bool ELFDynObjReader::readSymbols(Input& pInput)
{
  
  ELFObject<32>* rs_object = ELFReader::createELF32Object(pInput);
  llvm::OwningPtr<ELFObject<32> > own_ptr(rs_object);

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
                     << getSymbolTypeName(ld_type)
                     << "' of symbol `"
                     << ld_name.str()
                     << "'.\n";
        continue;
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

    /// convert to ResolveInfo::Visibility
    ResolveInfo::Visibility ld_vis = ELFReader::getVisibilityResolveInfo(rs_sym);

    m_Linker.addGlobalSymbol(ld_name,
                             true,
                             ld_type,
                             ld_desc,
                             ld_binding,
                             ld_size,
                             ld_frag_ref,
                             ld_vis);
  } // end of for

  return true;
}

