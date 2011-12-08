//===- ELFDSOWriter.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// LDContext stores the real content of a file.
// It is possible that multiple libraries reference to a same file,
// this shared file will be converted into a LDContext.
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_LD_LDCONTEXT_H
#define MCLD_LD_LDCONTEXT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/Support/FileSystem.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/ilist.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/MemoryBuffer.h"
#include <map>
#include <utility>

using namespace llvm;

typedef llvm::StringMap<const MCSectionELF*> ELFUniqueMapTy;

namespace llvm
{
  class MCSection;
  class MCSectionData;
} // namespace of llvm

namespace mcld
{

/** \class LDContext
 *  \brief LDContext stores the data which a object file should has
 */
class LDContext
{
public:
  LDContext() {
    ELFUniquingMap = 0;
  }

  ~LDContext() {}

  typedef iplist<MCSectionData> SectionDataListType;

  typedef SectionDataListType::const_iterator const_iterator;
  typedef SectionDataListType::iterator iterator;

  //Section List Access
  SectionDataListType Sections;

  const SectionDataListType &getSectionList() const { return Sections; }
  SectionDataListType &getSectionList() { return Sections; }

  iterator begin() { return Sections.begin(); }
  const_iterator begin() const { return Sections.begin(); }

  iterator end() { return Sections.end(); }
  const_iterator end() const { return Sections.end(); }

  size_t size() const { return Sections.size(); }


  // FIXME: Avoid this indirection?
  DenseMap<const MCSection*, MCSectionData*> SectionMap;

  void *ELFUniquingMap;

};


} // namespace of mcld

#endif


>>>>>>> Move MC/MCLDContext to LD/LDContext.
