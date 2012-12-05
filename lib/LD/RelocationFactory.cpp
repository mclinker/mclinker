//===- RelocationFactory.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/RelocationFactory.h>
#include <mcld/LinkerConfig.h>
#include <mcld/Target/TargetLDBackend.h>

#include <llvm/Support/Host.h>

#include <cstring>
#include <cassert>

using namespace mcld;

//===----------------------------------------------------------------------===//
// RelocationFactory
//===----------------------------------------------------------------------===//
RelocationFactory::RelocationFactory(size_t pNum, const LinkerConfig& pConfig)
  : GCFactory<Relocation, 0>(pNum), m_Config(pConfig) {
}

Relocation* RelocationFactory::produce(RelocationFactory::Type pType,
                                       FragmentRef& pFragRef,
                                       Address pAddend)
{
  // target_data is the place where the relocation applys to.
  // Use TargetDataFactory to generate temporary data, and copy the
  // content of the fragment into this data.
  DWord target_data = 0;

  // byte swapping if the host and target have different endian
  if(llvm::sys::isLittleEndianHost() != m_Config.targets().isLittleEndian()) {
     uint32_t tmp_data;

     if (m_Config.targets().is32Bits()) {
        pFragRef.memcpy(&tmp_data, 4);
        tmp_data = bswap32(tmp_data);
        target_data = tmp_data;
     }
     else if (m_Config.targets().is64Bits()) {
        pFragRef.memcpy(&target_data, 8);
        target_data = bswap64(target_data);
     }
  }
  else {
    pFragRef.memcpy(&target_data, (m_Config.targets().bitclass()/8));
  }

  Relocation *result = allocate();
  new (result) Relocation(pType, &pFragRef, pAddend, target_data);
  return result;
}

Relocation* RelocationFactory::produceEmptyEntry()
{
  // FIXME: To prevent relocations from double free by both iplist and
  // GCFactory, currently we new relocations directly and let iplist
  // delete them.

  return new Relocation(0, 0, 0, 0);
}

void RelocationFactory::destroy(Relocation* pRelocation)
{
   /** GCFactory will recycle the relocation **/
}

