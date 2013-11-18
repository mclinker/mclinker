//===- GarbageCollection.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/GarbageCollection.h>
#include <mcld/LinkerScript.h>
#include <mcld/Fragment/Fragment.h>
#include <mcld/Fragment/Relocation.h>
#include <mcld/LD/LDContext.h>
#include <mcld/LD/LDFileFormat.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/SectionData.h>
#include <mcld/LD/RelocData.h>
#include <mcld/LinkerConfig.h>
#include <mcld/Module.h>
#include <mcld/Support/MsgHandling.h>

#include <llvm/Support/Casting.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// GarbageCollection
//===----------------------------------------------------------------------===//
GarbageCollection::GarbageCollection(const LinkerConfig& pConfig,
                                     Module& pModule)
  : m_Config(pConfig), m_Module(pModule), m_pEntry(NULL)
{
}

GarbageCollection::~GarbageCollection()
{
}

bool GarbageCollection::run()
{
  // 1. traverse all the relocations to setup the reached sections
  setUpReachedSections();

  // 2. find all the referenced sections those can be reached by entry
  findReferencedSections();

  // 3. stripSections - set the unreached sections to Ignore
  stripSections();
  return true;
}

void GarbageCollection::setUpReachedSections()
{
  // traverse all the input relocations to setup the reached sections
  Module::obj_iterator input, inEnd = m_Module.obj_end();
  for (input = m_Module.obj_begin(); input != inEnd; ++input) {
    LDContext::sect_iterator rs, rsEnd = (*input)->context()->relocSectEnd();
    for (rs = (*input)->context()->relocSectBegin(); rs != rsEnd; ++rs) {
      // bypass the discarded relocation section
      // 1. its section kind is changed to Ignore. (The target section is a
      // discarded group section.)
      // 2. it has no reloc data. (All symbols in the input relocs are in the
      // discarded group sections)
      LDSection* reloc_sect = *rs;
      LDSection* apply_sect = reloc_sect->getLink();
      if ((LDFileFormat::Ignore == reloc_sect->kind()) ||
          (!reloc_sect->hasRelocData()))
        continue;

      // bypass the apply target sections which are not handled by gc (currently
      // we only handle the Regular and BSS sections)
      if (apply_sect->kind() != LDFileFormat::Regular &&
          apply_sect->kind() != LDFileFormat::BSS)
        continue;

      bool add_first = false;
      SectionListTy* reached_sects = NULL;
      RelocData::iterator reloc_it, rEnd = reloc_sect->getRelocData()->end();
      for (reloc_it = reloc_sect->getRelocData()->begin(); reloc_it != rEnd;
                                                                   ++reloc_it) {
        Relocation* reloc = llvm::cast<Relocation>(reloc_it);
        ResolveInfo* sym = reloc->symInfo();
        // only the target symbols defined in the input fragments can make the
        // reference
        if (NULL == sym)
          continue;
        if (!sym->isDefine() || !sym->outSymbol()->hasFragRef())
          continue;

        // only the target symbols defined in the concerned sections can make
        // the reference
        const LDSection* target_sect =
                &sym->outSymbol()->fragRef()->frag()->getParent()->getSection();
        if (target_sect->kind() != LDFileFormat::Regular &&
            target_sect->kind() != LDFileFormat::BSS)
          continue;

        // setup the reached list, if we first add the element to reached list
        // of this section, create an entry in ReachedSections map
        if (!add_first) {
          assert(NULL == reached_sects);
          reached_sects = &m_ReachedSections[apply_sect];
          add_first = true;
        }
        reached_sects->insert(target_sect);
      }
      reached_sects = NULL;
      add_first = false;
    }
  }
}

void GarbageCollection::getEntrySections(SectionVecTy& pEntry)
{
  // all the KEEP sections defined in ldscript are entries, traverse all the
  // input sections and check the SectionMap to find the KEEP sections
  Module::obj_iterator obj, objEnd = m_Module.obj_end();
  SectionMap& sect_map = m_Module.getScript().sectionMap();
  for (obj = m_Module.obj_begin(); obj != objEnd; ++obj) {
    const std::string input_name = (*obj)->name();
    LDContext::sect_iterator sect, sectEnd = (*obj)->context()->sectEnd();
    for (sect = (*obj)->context()->sectBegin(); sect != sectEnd; ++sect) {
      LDSection* section = *sect;
      if (LDFileFormat::Regular != section->kind() &&
          LDFileFormat::BSS != section->kind())
        continue;

      SectionMap::Input* sm_input =
                              sect_map.find(input_name, section->name()).second;
      if ((sm_input != NULL) && (InputSectDesc::Keep == sm_input->policy()))
        pEntry.push_back(section);
    }
  }

  // get the sections those the entry symbols defined in
  Module::SymbolTable& sym_tab = m_Module.getSymbolTable();
  if (LinkerConfig::Exec == m_Config.codeGenType()) {
    assert(NULL != m_pEntry);
    pEntry.push_back(&m_pEntry->fragRef()->frag()->getParent()->getSection());
  }
  else {
    // when building shared objects, the global define symbols are entries
    SymbolCategory::iterator it, end = sym_tab.regularEnd();
    for (it = sym_tab.dynamicBegin(); it != end; ++it) {
      LDSymbol* sym = *it;
      if (!sym->resolveInfo()->isDefine() || !sym->hasFragRef())
        continue;

      // only the target symbols defined in the concerned sections can make
      // the reference
      const LDSection* sect =
                             &sym->fragRef()->frag()->getParent()->getSection();
      if (sect->kind() != LDFileFormat::Regular &&
          sect->kind() != LDFileFormat::BSS)
        continue;

      pEntry.push_back(sect);
    }
  }
}

void GarbageCollection::findReferencedSections()
{
  // find all sections reached by entry symbols
  SectionVecTy entry;
  getEntrySections(entry);

  // list of sections waiting to be processed
  WorkListTy work_list;
  // start from each entry, resolve the transitive closure
  SectionVecTy::iterator entry_it, entry_end = entry.end();
  for (entry_it = entry.begin(); entry_it != entry_end; ++entry_it) {
    // add entry point to work list
    work_list.push(*entry_it);

    // add section from the work_list to the referencedSections until every
    // reached sections are added
    while (!work_list.empty()) {
      const LDSection* sect = work_list.front();
      work_list.pop();
      // add section to the ReferencedSections, if the section has been put into
      // referencedSections, skip this section
      if (!m_ReferencedSections.insert(sect).second)
        continue;

      // get the section reached list, if the section do not has one, which
      // means no referenced between it and other sections, then skip it
      ReachedSectionsTy::iterator reach_it = m_ReachedSections.find(sect);
      if (reach_it == m_ReachedSections.end())
        continue;

      // put the reached sections to work list, skip the one already be in
      // referencedSections
      SectionListTy& reach_list = reach_it->second;
      SectionListTy::iterator it, end = reach_list.end();
      for (it = reach_list.begin(); it != end; ++it) {
        if (m_ReferencedSections.find(*it) == m_ReferencedSections.end())
          work_list.push(*it);
      }
    }
  }
}

void GarbageCollection::stripSections()
{
  // Traverse all the input Regular and BSS sections, if a section is not found
  // in the ReferencedSections, then it should be garbage collected
  Module::obj_iterator obj, objEnd = m_Module.obj_end();
  for (obj = m_Module.obj_begin(); obj != objEnd; ++obj) {
    LDContext::sect_iterator sect, sectEnd = (*obj)->context()->sectEnd();
    for (sect = (*obj)->context()->sectBegin(); sect != sectEnd; ++sect) {
      LDSection* section = *sect;
      if (LDFileFormat::Regular != section->kind() &&
          LDFileFormat::BSS != section->kind())
        continue;

      if (m_ReferencedSections.find(section) == m_ReferencedSections.end())
        section->setKind(LDFileFormat::Ignore);
    }
  }

  // Traverse all the relocation sections, if its target section is set to
  // Ignore, then set the relocation section to Ignore as well
  Module::obj_iterator input, inEnd = m_Module.obj_end();
  for (input = m_Module.obj_begin(); input != inEnd; ++input) {
    LDContext::sect_iterator rs, rsEnd = (*input)->context()->relocSectEnd();
    for (rs = (*input)->context()->relocSectBegin(); rs != rsEnd; ++rs) {
      LDSection* reloc_sect = *rs;
      if (LDFileFormat::Ignore == reloc_sect->getLink()->kind())
        reloc_sect->setKind(LDFileFormat::Ignore);
    }
  }
}

