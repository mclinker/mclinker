/*****************************************************************************
 *   The BOLD Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@gmail.com>                                            *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#include <llvm/Support/raw_ostream.h>
#include <mcld/MC/RelocData.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/MC/MCLDLayout.h>
#include <mcld/MC/MCLDFile.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/Target/TargetLDBackend.h>

namespace mcld {

MCLinker::MCLinker(TargetLDBackend& pBackend, MCLDInfo& pInfo)
: m_pBackend(pBackend), m_pInfo(pInfo), m_pRelocData(NULL)
{
}

MCLinker::~MCLinker()
{
  delete m_pRelocData;
  m_pRelocData = NULL;
}

void MCLinker::addCommand( MCLDCommand& pCommand )
{
}

void MCLinker::setLayout( MCLDLayout& pLayout )
{
}

void MCLinker::addLdFile( MCLDFile& pLDFile )
{
}

void MCLinker::relocate() {
  read_relocs();
  scan_relocs();
  relocate_relocs();
}

void MCLinker::read_relocs() {
  InputTree& Tree = m_pInfo.inputs();
  for (InputTree::const_iterator i = Tree.begin(), e = Tree.end();
       i != e; ++i) {
    const Input* In = *i;
    assert(In->hasContext());
    const MCLDContext* Ctx = In->context();

    m_pRelocData->relocs.clear();
    unsigned int shnum = Ctx->Sections.size();
    if (shnum == 0) {
      return;
    }

    m_pRelocData->relocs.reserve(shnum/2);
  }
}

void MCLinker::scan_relocs() {
}

void MCLinker::relocate_relocs() {
}

} // namespace mcld
