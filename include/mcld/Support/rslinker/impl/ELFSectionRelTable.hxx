/*
 * Copyright 2011, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ELF_SECTION_REL_TABLE_HXX
#define ELF_SECTION_REL_TABLE_HXX

#include "mcld/Support/rslinker/ELFReloc.h"

template <unsigned Bitwidth>
ELFSectionRelTable<Bitwidth>::~ELFSectionRelTable() {
  using namespace std;
  for (size_t i = 0; i < table.size(); ++i) {
    delete table[i];
  }
}

template <unsigned Bitwidth>
template <typename Archiver>
ELFSectionRelTable<Bitwidth> *
ELFSectionRelTable<Bitwidth>::read(Archiver &AR,
                                   ELFSectionHeaderTy const *sh) {

  assert(sh->getType() == SHT_REL || sh->getType() == SHT_RELA);

  llvm::OwningPtr<ELFSectionRelTable> rt(new ELFSectionRelTable());

  // Seek to the start of the table
  AR.seek(sh->getOffset(), true);

  // Count the relocation entries
  size_t size = sh->getSize() / sh->getEntrySize();

  // Read every relocation entries
  if (sh->getType() == SHT_REL) {
    assert(sh->getEntrySize() == TypeTraits<ELFRelocRelTy>::size);
    for (size_t i = 0; i < size; ++i) {
      rt->table.push_back(ELFRelocTy::readRel(AR, i));
    }

  } else {
    assert(sh->getEntrySize() == TypeTraits<ELFRelocRelaTy>::size);
    for (size_t i = 0; i < size; ++i) {
      rt->table.push_back(ELFRelocTy::readRela(AR, i));
    }
  }

  if (!AR) {
    // Unable to read the table.
    return 0;
  }

  return rt.take();
}

#endif // ELF_SECTION_REL_TABLE_HXX
