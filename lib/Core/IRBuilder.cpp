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
#include <mcld/Support/MsgHandling.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// IRBuilder
//===----------------------------------------------------------------------===//
IRBuilder::IRBuilder(Module& pModule,
                     InputTree& pInputs,
                     const LinkerConfig& pConfig)
  : m_Module(pModule), m_InputTree(pInputs), m_Config(pConfig),
    m_InputBuilder(pConfig) {
  m_InputBuilder.setCurrentTree(m_InputTree);
}

IRBuilder::~IRBuilder()
{
}

Input* IRBuilder::CreateInput(const sys::fs::Path& pPath, unsigned int pType)
{
  m_InputBuilder.createNode<InputTree::Positional>(pPath.filename().native(),
                                                   pPath,
                                                   pType);
  Input* input = *m_InputBuilder.getCurrentNode();

  if (!input->hasContext())
    m_InputBuilder.setContext(*input);

  if (!input->hasMemArea())
    m_InputBuilder.setMemory(*input, FileHandle::ReadOnly, FileHandle::System);

  return input;
}

Input* IRBuilder::CreateInput(const std::string& pName,
                              const sys::fs::Path& pPath,
                              unsigned int pType)
{
  m_InputBuilder.createNode<InputTree::Positional>(pName, pPath, pType);
  Input* input = *m_InputBuilder.getCurrentNode();

  if (!input->hasContext())
    m_InputBuilder.setContext(*input);

  if (!input->hasMemArea())
    m_InputBuilder.setMemory(*input, FileHandle::ReadOnly, FileHandle::System);

  return input;
}

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
    return false;
  }

  m_InputBuilder.createNode<InputTree::Positional>(pNameSpec, *path);
  Input* input = *m_InputBuilder.getCurrentNode();

  if (!input->hasContext())
    m_InputBuilder.setContext(*input);

  if (!input->hasMemArea())
    m_InputBuilder.setMemory(*input, FileHandle::ReadOnly, FileHandle::System);

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

template<> LDSection*
IRBuilder::CreateSection<IRBuilder::ELF>(Input& pInput,
                                         const std::string& pName,
                                         uint32_t pType,
                                         uint32_t pFlag,
                                         uint32_t pAlign)
{
  // Create section header
  LDFileFormat::Kind kind = ELFReaderIF::GetSectionKind(pType, pName.c_str());
  LDSection* header = LDSection::Create(pName, kind, pType, pFlag);
  header->setAlign(pAlign);

  // Append section header in input
  pInput.context()->appendSection(*header);

  // Create section data
  switch (kind) {
    case LDFileFormat::EhFrameHdr:
    case LDFileFormat::NamePool:
    case LDFileFormat::Null:
      // ignore
      break;
    case LDFileFormat::Group:
      // not support yet
      break;
    case LDFileFormat::Relocation:
      ObjectBuilder::CreateRelocData(*header);
      break;
    case LDFileFormat::EhFrame:
      ObjectBuilder::CreateEhFrame(*header);
      break;
    case LDFileFormat::BSS:
      // We does not know the size of BSS until appending fragment.
      /** fall through **/
    default:
      ObjectBuilder::CreateSectionData(*header);
      break;
  }
}

/// CreateRegion - To create a region fragment in the input file.
RegionFragment*
IRBuilder::CreateRegion(Input& pInput, size_t pOffset, size_t pLength)
{
  if (!pInput.hasMemArea()) {
    fatal(diag::fatal_cannot_read_input) << pInput.path();
    return NULL;
  }

  MemoryRegion* region = pInput.memArea()->request(pOffset, pLength);
  RegionFragment* frag = new RegionFragment(*region);
  return frag;
}

/// CreateRegion - To create a region fragment wrapping the given memory
RegionFragment*
IRBuilder::CreateRegion(void* pMemory, size_t pLength)
{
  MemoryRegion* region = MemoryRegion::Create(pMemory, pLength);
  RegionFragment* frag = new RegionFragment(*region);
  return frag;
}

/// AppendFragment - To append an fragment in the section.
bool IRBuilder::AppendFragment(Fragment& pFrag, LDSection& pSection)
{
  return ObjectBuilder::AppendFragment(pFrag, pSection);
}

