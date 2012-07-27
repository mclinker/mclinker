//===- ELFSectionMap.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/MsgHandling.h>
#include <mcld/LD/ELFSectionMap.h>

using namespace mcld;

struct NameMap {
  const char* from;
  const char* to;
};

// Common mappings of ELF and other formants. Now only ELF specific mappings
// are added
static const NameMap map[] =
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
  // TODO: Support DT_INIT_ARRAY for all constructors?
  {".ctors", ".ctors"},
  {".dtors", ".dtors"},
  {".sdata", ".sdata"},
  {".sbss", ".sbss"},
  // FIXME: in GNU ld, if we are creating a shared object .sdata2 and .sbss2
  // sections would be handled differently.
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
};

static const int map_size = (sizeof(map) / sizeof(map[0]));

ELFSectionMap::ELFSectionMap()
{
}

ELFSectionMap::~ELFSectionMap()
{
}

void ELFSectionMap::initStandardMaps()
{
  for (unsigned int i = 0; i < map_size; ++i) {
    bool exist = false;
    NamePair& pair = append(map[i].from, map[i].to, exist);
    if (exist) {
      warning(diag::warn_duplicate_std_sectmap) << map[i].from
                                                << map[i].to
                                                << pair.from
                                                << pair.to;
    }
  }
}

