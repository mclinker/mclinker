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

#ifndef ELF_SECTION_STR_TAB_HXX
#define ELF_SECTION_STR_TAB_HXX

#include <llvm/ADT/OwningPtr.h>

template <unsigned Bitwidth>
template <typename Archiver>
ELFSectionStrTab<Bitwidth> *
ELFSectionStrTab<Bitwidth>::read(Archiver &AR,
                                 ELFSectionHeaderTy const *sh) {

  llvm::OwningPtr<ELFSectionStrTab> st(new ELFSectionStrTab());
  st->buf.resize(sh->getSize());

  // Save section_header
  st->section_header = sh;

  AR.seek(sh->getOffset(), true);
  AR.prologue(sh->getSize());
  AR.readBytes(&*st->buf.begin(), sh->getSize());
  AR.epilogue(sh->getSize());

  if (!AR) {
    // Unable to read the string table.
    return 0;
  }

  return st.take();
}

#endif // ELF_SECTION_STR_TAB_HXX
