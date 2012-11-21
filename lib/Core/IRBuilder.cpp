//===- IRBuilder.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/IRBuilder.h>
#include <mcld/LD/ELFReader.h>
#include <mcld/Object/ObjectBuilder.h>
#include <mcld/LD/SectionData.h>
#include <mcld/LD/EhFrame.h>
#include <mcld/LD/RelocData.h>
#include <mcld/Support/MsgHandling.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// Helper Functions
//===----------------------------------------------------------------------===//
LDFileFormat::Kind GetELFSectionKind(uint32_t pType, const char* pName)
{
  // name rules
  llvm::StringRef name(pName);
  if (name.startswith(".debug") ||
      name.startswith(".zdebug") ||
      name.startswith(".gnu.linkonce.wi.") ||
      name.startswith(".line") ||
      name.startswith(".stab"))
    return LDFileFormat::Debug;
  if (name.startswith(".comment"))
    return LDFileFormat::MetaData;
  if (name.startswith(".interp") || name.startswith(".dynamic"))
    return LDFileFormat::Note;
  if (name.startswith(".eh_frame"))
    return LDFileFormat::EhFrame;
  if (name.startswith(".eh_frame_hdr"))
    return LDFileFormat::EhFrameHdr;
  if (name.startswith(".gcc_except_table"))
    return LDFileFormat::GCCExceptTable;
  if (name.startswith(".note.GNU-stack"))
    return LDFileFormat::StackNote;

  // type rules
  switch(pType) {
  case llvm::ELF::SHT_NULL:
    return LDFileFormat::Null;
  case llvm::ELF::SHT_INIT_ARRAY:
  case llvm::ELF::SHT_FINI_ARRAY:
  case llvm::ELF::SHT_PREINIT_ARRAY:
  case llvm::ELF::SHT_PROGBITS:
    return LDFileFormat::Regular;
  case llvm::ELF::SHT_SYMTAB:
  case llvm::ELF::SHT_DYNSYM:
  case llvm::ELF::SHT_STRTAB:
  case llvm::ELF::SHT_HASH:
  case llvm::ELF::SHT_DYNAMIC:
    return LDFileFormat::NamePool;
  case llvm::ELF::SHT_RELA:
  case llvm::ELF::SHT_REL:
    return LDFileFormat::Relocation;
  case llvm::ELF::SHT_NOBITS:
    return LDFileFormat::BSS;
  case llvm::ELF::SHT_NOTE:
    return LDFileFormat::Note;
  case llvm::ELF::SHT_GROUP:
    return LDFileFormat::Group;
  case llvm::ELF::SHT_GNU_versym:
  case llvm::ELF::SHT_GNU_verdef:
  case llvm::ELF::SHT_GNU_verneed:
    return LDFileFormat::Version;
  case llvm::ELF::SHT_SHLIB:
    return LDFileFormat::Target;
  default:
    if ((pType >= llvm::ELF::SHT_LOPROC && pType <= llvm::ELF::SHT_HIPROC) ||
        (pType >= llvm::ELF::SHT_LOOS && pType <= llvm::ELF::SHT_HIOS) ||
        (pType >= llvm::ELF::SHT_LOUSER && pType <= llvm::ELF::SHT_HIUSER))
      return LDFileFormat::Target;
    fatal(diag::err_unsupported_section) << pName << pType;
  }
  return LDFileFormat::MetaData;
}

//===----------------------------------------------------------------------===//
// IRBuilder
//===----------------------------------------------------------------------===//
IRBuilder::IRBuilder(Module& pModule, const LinkerConfig& pConfig)
  : m_Module(pModule), m_Config(pConfig), m_InputBuilder(pConfig) {
  m_InputBuilder.setCurrentTree(m_Module.getInputTree());
}

IRBuilder::~IRBuilder()
{
}

/// CreateInput - To create an input file and append it to the input tree.
Input* IRBuilder::CreateInput(const std::string& pName,
                              const sys::fs::Path& pPath, Input::Type pType)
{
  if (Input::Unknown == pType)
    return ReadInput(pName, pPath);

  m_InputBuilder.createNode<InputTree::Positional>(pName, pPath, pType);
  Input* input = *m_InputBuilder.getCurrentNode();

  if (!input->hasContext())
    m_InputBuilder.setContext(*input, false);

  return input;
}

/// ReadInput - To read an input file and append it to the input tree.
Input*
IRBuilder::ReadInput(const std::string& pName, const sys::fs::Path& pPath)
{
  m_InputBuilder.createNode<InputTree::Positional>(pName, pPath, Input::Unknown);
  Input* input = *m_InputBuilder.getCurrentNode();

  if (!input->hasContext())
    m_InputBuilder.setContext(*input);

  if (!input->hasMemArea())
    m_InputBuilder.setMemory(*input, FileHandle::ReadOnly, FileHandle::System);

  return input;
}

/// ReadInput - To read an input file and append it to the input tree.
Input* IRBuilder::ReadInput(const std::string& pNameSpec)
{
  const sys::fs::Path* path = NULL;
  // find out the real path of the namespec.
  if (m_InputBuilder.getConstraint().isSharedSystem()) {
    // In the system with shared object support, we can find both archive
    // and shared object.

    if (m_InputBuilder.getAttributes().isStatic()) {
      // with --static, we must search an archive.
      path = m_Config.options().directories().find(pNameSpec, Input::Archive);
    }
    else {
      // otherwise, with --Bdynamic, we can find either an archive or a
      // shared object.
      path = m_Config.options().directories().find(pNameSpec, Input::DynObj);
    }
  }
  else {
    // In the system without shared object support, we only look for an archive
    path = m_Config.options().directories().find(pNameSpec, Input::Archive);
  }

  if (NULL == path) {
    fatal(diag::err_cannot_find_namespec) << pNameSpec;
    return NULL;
  }

  m_InputBuilder.createNode<InputTree::Positional>(pNameSpec, *path);
  Input* input = *m_InputBuilder.getCurrentNode();

  if (!input->hasContext())
    m_InputBuilder.setContext(*input);

  if (!input->hasMemArea())
    m_InputBuilder.setMemory(*input, FileHandle::ReadOnly, FileHandle::System);

  return input;
}

/// ReadInput - To read an input file and append it to the input tree.
Input* IRBuilder::ReadInput(raw_mem_ostream& pMemOStream)
{
  Input* input = NULL;
  if (pMemOStream.getMemoryArea().hasHandler()) {
    m_InputBuilder.createNode<InputTree::Positional>(
                               "memory ostream",
                               pMemOStream.getMemoryArea().handler()->path());

    input = *m_InputBuilder.getCurrentNode();
    m_InputBuilder.setContext(*input);
    input->setMemArea(&pMemOStream.getMemoryArea());
  }
  else {
    m_InputBuilder.createNode<InputTree::Positional>("memory ostream", "NAN");
    input = *m_InputBuilder.getCurrentNode();
    m_InputBuilder.setContext(*input, false);
    input->setMemArea(&pMemOStream.getMemoryArea());
  }

  return input;
}

/// ReadInput - To read an input file and append it to the input tree.
Input* IRBuilder::ReadInput(FileHandle& pFileHandle)
{
  m_InputBuilder.createNode<InputTree::Positional>("file handler",
                                                   pFileHandle.path());

  Input* input = *m_InputBuilder.getCurrentNode();
  if (pFileHandle.path().empty()) {
    m_InputBuilder.setContext(*input, false);
    m_InputBuilder.setMemory(*input, pFileHandle.handler(), FileHandle::ReadOnly);
  }
  else {
    m_InputBuilder.setContext(*input, true);
    m_InputBuilder.setMemory(*input, FileHandle::ReadOnly, FileHandle::System);
  }

  return input;
}

/// ReadInput - To read an input file and append it to the input tree.
Input* IRBuilder::ReadInput(const std::string& pName, void* pRawMemory, size_t pSize)
{
  m_InputBuilder.createNode<InputTree::Positional>(pName, "NAN");
  Input* input = *m_InputBuilder.getCurrentNode();
  m_InputBuilder.setContext(*input, false);
  m_InputBuilder.setMemory(*input, pRawMemory, pSize);
  return input;
}

bool IRBuilder::StartGroup()
{
  if (m_InputBuilder.isInGroup()) {
    fatal(diag::fatal_forbid_nest_group);
    return false;
  }
  m_InputBuilder.enterGroup();
  return true;
}

bool IRBuilder::EndGroup()
{
  m_InputBuilder.exitGroup();
  return true;
}

void IRBuilder::WholeArchive()
{
  m_InputBuilder.getAttributes().setWholeArchive();
}

void IRBuilder::NoWholeArchive()
{
  m_InputBuilder.getAttributes().unsetWholeArchive();
}

void IRBuilder::AsNeeded()
{
  m_InputBuilder.getAttributes().setAsNeeded();
}

void IRBuilder::NoAsNeeded()
{
  m_InputBuilder.getAttributes().unsetAsNeeded();
}

void IRBuilder::CopyDTNeeded()
{
  m_InputBuilder.getAttributes().setAddNeeded();
}

void IRBuilder::NoCopyDTNeeded()
{
  m_InputBuilder.getAttributes().unsetAddNeeded();
}

void IRBuilder::AgainstShared()
{
  m_InputBuilder.getAttributes().setDynamic();
}

void IRBuilder::AgainstStatic()
{
  m_InputBuilder.getAttributes().setStatic();
}

LDSection* IRBuilder::CreateELFHeader(Input& pInput,
                                      const std::string& pName,
                                      uint32_t pType,
                                      uint32_t pFlag,
                                      uint32_t pAlign)
{
  // Create section header
  LDFileFormat::Kind kind = GetELFSectionKind(pType, pName.c_str());
  LDSection* header = LDSection::Create(pName, kind, pType, pFlag);
  header->setAlign(pAlign);

  // Append section header in input
  pInput.context()->appendSection(*header);
  return header;
}

/// CreateSectionData - To create a section data for given pSection.
SectionData* IRBuilder::CreateSectionData(LDSection& pSection)
{
  assert(!pSection.hasSectionData() && "pSection already has section data.");

  SectionData* sect_data = SectionData::Create(pSection);
  pSection.setSectionData(sect_data);
  return sect_data;
}

/// CreateRelocData - To create a relocation data for given pSection.
RelocData* IRBuilder::CreateRelocData(LDSection &pSection)
{
  assert(!pSection.hasRelocData() && "pSection already has relocation data.");

  RelocData* reloc_data = RelocData::Create(pSection);
  pSection.setRelocData(reloc_data);
  return reloc_data;
}

/// CreateEhFrame - To create a eh_frame for given pSection
EhFrame* IRBuilder::CreateEhFrame(LDSection& pSection)
{
  assert(!pSection.hasEhFrame() && "pSection already has eh_frame.");

  EhFrame* eh_frame = new EhFrame(pSection);
  pSection.setEhFrame(eh_frame);
  return eh_frame;
}

/// CreateBSS - To create a bss section for given pSection
SectionData* IRBuilder::CreateBSS(LDSection& pSection)
{
  assert(!pSection.hasSectionData() && "pSection already has section data.");
  assert((pSection.kind() == LDFileFormat::BSS) && "pSection is not a BSS section.");

  SectionData* sect_data = SectionData::Create(pSection);
  pSection.setSectionData(sect_data);

                                   /*  value, valsize, size*/
  FillFragment* frag = new FillFragment(0x0, 1, pSection.size());

  ObjectBuilder::AppendFragment(*frag, *sect_data);
  return sect_data;
}

/// CreateRegion - To create a region fragment in the input file.
Fragment* IRBuilder::CreateRegion(Input& pInput, size_t pOffset, size_t pLength)
{
  if (!pInput.hasMemArea()) {
    fatal(diag::fatal_cannot_read_input) << pInput.path();
    return NULL;
  }

  if (0 == pLength)
    return new FillFragment(0x0, 0, 0);

  MemoryRegion* region = pInput.memArea()->request(pOffset, pLength);

  if (NULL == region)
    return new FillFragment(0x0, 0, 0);

  return new RegionFragment(*region);
}

/// CreateRegion - To create a region fragment wrapping the given memory
Fragment* IRBuilder::CreateRegion(void* pMemory, size_t pLength)
{
  if (0 == pLength)
    return new FillFragment(0x0, 0, 0);

  MemoryRegion* region = MemoryRegion::Create(pMemory, pLength);
  if (NULL == region)
    return new FillFragment(0x0, 0, 0);

  return new RegionFragment(*region);
}

/// AppendFragment - To append pFrag to the given SectionData pSD
uint64_t IRBuilder::AppendFragment(Fragment& pFrag, SectionData& pSD)
{
  uint64_t size = ObjectBuilder::AppendFragment(pFrag,
                                                pSD,
                                                pSD.getSection().align());
  pSD.getSection().setSize(pSD.getSection().size() + size);
  return size;
}

/// AppendRelocation - To append an relocation to the given RelocData pRD.
void IRBuilder::AppendRelocation(Relocation& pRelocation, RelocData& pRD)
{
  pRD.getFragmentList().push_back(&pRelocation);
}

/// AppendEhFrame - To append a fragment to EhFrame.
uint64_t IRBuilder::AppendEhFrame(Fragment& pFrag, EhFrame& pEhFrame)
{
  uint64_t size = ObjectBuilder::AppendFragment(pFrag,
                              pEhFrame.getSectionData(),
                              pEhFrame.getSection().align());
  pEhFrame.getSection().setSize(pEhFrame.getSection().size() + size);
  return size;
}

/// AppendEhFrame - To append a FDE to the given EhFrame pEhFram.
uint64_t IRBuilder::AppendEhFrame(EhFrame::FDE& pFDE, EhFrame& pEhFrame)
{
  pEhFrame.addFDE(pFDE);
  pEhFrame.getSection().setSize(pEhFrame.getSection().size() + pFDE.size());
  return pFDE.size();
}

/// AppendEhFrame - To append a CIE to the given EhFrame pEhFram.
uint64_t IRBuilder::AppendEhFrame(EhFrame::CIE& pCIE, EhFrame& pEhFrame)
{
  pEhFrame.addCIE(pCIE);
  pEhFrame.getSection().setSize(pEhFrame.getSection().size() + pCIE.size());
  return pCIE.size();
}

