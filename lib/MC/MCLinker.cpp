//===- MCLinker.cpp -------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "llvm/Support/raw_ostream.h"
#include "mcld/MC/MCLinker.h"
#include "mcld/MC/MCLDLayout.h"
#include "mcld/MC/MCLDInput.h"
#include "mcld/MC/MCLDInfo.h"
#include "mcld/MC/MCLDContext.h"
#include "mcld/Target/TargetLDBackend.h"

namespace mcld {

MCLinker::MCLinker(TargetLDBackend& pBackend, MCLDInfo& pInfo)
: m_pBackend(pBackend), m_pInfo(pInfo)
{
}

MCLinker::~MCLinker()
{
}

// FIXME(Nowar): Take this away
#if 0
void MCLinker::init() {
  m_pReloc = new RelocData();
  InputTree& Tree = m_pInfo.inputs();
  // Transfer the data structure from Input tp RelocData
  for (InputTree::const_iterator i = Tree.begin(), e = Tree.end(); i != e; ++i) {
    const Input* In = *i;

    if (!In->hasContext())  continue;
    const MCLDContext* Ctx = In->context();
    for (MCLDContext::const_iterator i = Ctx->begin(), e = Ctx->end(); i != e; ++i) {
      const MCSectionData& SD = *i;
      const MCSection& Sec = SD.getSection();
      for (MCSectionData::const_iterator i = SD.begin(), e = SD.end(); i != e; ++i) {
        const MCFragment& Frag = *i;
        if (Frag.getKind() == MCFragment::FT_Data || Frag.getKind() == MCFragment::FT_Inst) {
        }
      }
    }

#if 0
    SectionRelocData* Data = new SectionRelocData();
    Data->m_FileName = In->InputName;
    Data->m_pAttr = In->m_pAttr;
    Data->m_Type = In->m_Type;

    m_pReloc->m_Data.push_back(Data);
#endif
  }
}
#endif

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
  // TODO(Nowar)
  //m_pBackend.relocate(...);
}

} // namespace mcld
