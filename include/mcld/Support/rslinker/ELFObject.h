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

#ifndef ELF_OBJECT_H
#define ELF_OBJECT_H

#include "ELFTypes.h"

#include <llvm/ADT/OwningPtr.h>

#include <string>
#include <vector>

template <unsigned Bitwidth>
class ELFObject {
public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

private:
  llvm::OwningPtr<ELFHeaderTy> header;
  llvm::OwningPtr<ELFSectionHeaderTableTy> shtab;
  std::vector<ELFSectionTy *> stab;

private:
  ELFObject() { }

public:
  template <typename Archiver>
  static ELFObject *read(Archiver &AR);

  ELFHeaderTy const *getHeader() const {
    return header.get();
  }

  ELFSectionHeaderTableTy const *getSectionHeaderTable() const {
    return shtab.get();
  }

  size_t getSectionNumber() const {
    return stab.size();
  }

  char const *getSectionName(size_t i) const;
  ELFSectionTy const *getSectionByIndex(size_t i) const;
  ELFSectionTy *getSectionByIndex(size_t i);
  ELFSectionTy const *getSectionByName(std::string const &str) const;
  ELFSectionTy *getSectionByName(std::string const &str);

  void print() const;

  ~ELFObject() {
    for (size_t i = 0; i < stab.size(); ++i) {
      // Delete will check the pointer is nullptr or not by himself.
      delete stab[i];
    }
  }
};

#include "impl/ELFObject.hxx"

#endif // ELF_OBJECT_H
