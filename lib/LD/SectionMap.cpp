//===- SectionMap.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <cassert>
#include <cstring>
#include <mcld/LD/SectionMap.h>

using namespace mcld;

//==========================
// SectionMap

SectionMap::SectionMap()
{
  initializeMap();
}

SectionMap::~SectionMap()
{}

bool SectionMap::addNameMapping(const std::string& pFrom, const std::string& pTo)
{
  iterator it;
  // check if the mapping exists in the map already
  for (it = begin(); it != end(); ++it) {
    if (pFrom == (*it).first)
      return false;
  }
  m_NameMap.push_back(NamePairTy(pFrom, pTo));
  return true;
}

const std::string& SectionMap::getOutputSectionName(const std::string& pFrom)
{
  iterator it;
  for (it = begin(); it != end(); ++it) {
    if (0 == strncmp((*it).first.c_str(), pFrom.c_str(), (*it).first.length()))
      break;
  }
  assert(end() != it);
  return (*it).second;
}

// general mappings for elf format.
const SectionMap::SectionNameMapping SectionMap::m_GeneralMap[] =
{
  {".text", ".text"},
  {".rodata", ".rodata"},
  {".data.rel.ro.local", ".data.rel.ro.local"},
  {".data.rel.ro", ".data.rel.ro"},
  {".data", ".data"},
  {".bss", ".bss"},
  {".tdata", ".tdata"},
  {".tbss", ".tbss"},
  {".init_array", ".init_array"},
  {".fini_array", ".fini_array"},
  {".sdata", ".sdata"},
  {".sbss", ".sbss"},
  // FIXME: In the GNU linker, .sbss2 and .sdata2 are handled
  // differently depending on whether it is creating a shared library.
  {".sdata2", ".sdata"},
  {".sbss2", ".sbss"},
  {".lrodata", ".lrodata"},
  {".ldata", ".ldata"},
  {".lbss", ".lbss"},
  {".gcc_except_table", ".gcc_except_table"},
  {".gnu.linkonce.d.rel.ro.local", ".data.rel.ro.local"},
  {".gnu.linkonce.d.rel.ro", ".data.rel.ro"},
  {".gnu.linkonce.t", ".text"},
  {".gnu.linkonce.r", ".rodata"},
  {".gnu.linkonce.d", ".data"},
  {".gnu.linkonce.b", ".bss"},
  {".gnu.linkonce.s", ".sdata"},
  {".gnu.linkonce.sb", ".sbss"},
  {".gnu.linkonce.s2", ".sdata"},
  {".gnu.linkonce.sb2", ".sbss"},
  {".gnu.linkonce.wi", ".debug_info"},
  {".gnu.linkonce.td", ".tdata"},
  {".gnu.linkonce.tb", ".tbss"},
  {".gnu.linkonce.lr", ".lrodata"},
  {".gnu.linkonce.l", ".ldata"},
  {".gnu.linkonce.lb", ".lbss"},
  // FIXME: add target dependent mappings in Backend.
  {".ARM.extab", ".ARM.extab"},
  {".gnu.linkonce.armextab", ".ARM.extab"},
  {".ARM.exidx", ".ARM.exidx"},
  {".gnu.linkonce.armexidx", ".ARM.exidx"},
};

const int SectionMap::m_GeneralMapSize =
  (sizeof(SectionMap::m_GeneralMap) / sizeof(SectionMap::m_GeneralMap[0]));

void SectionMap::initializeMap()
{
  for (int i = 0; i < m_GeneralMapSize; ++i)
    m_NameMap.push_back(NamePairTy(std::string(m_GeneralMap[i].from),
                                   std::string(m_GeneralMap[i].to)));
  assert(m_GeneralMapSize == m_NameMap.size());
}
