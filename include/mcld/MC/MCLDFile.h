/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCLDFILE_H
#define MCLDFILE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/MC/MCLDContext.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/Path.h>
#include <mcld/ADT/Allocators.h>
#include <string>
#include <list>
#include <memory>

#include "mcld/MC/MCLDContext.h"

namespace mcld
{
class MCContext;
class MCLDContext;

/** \class MCLDFile
 *  \brief MCLDFile represents the file being linked or produced.
 *
 *  \see llvm::sys::Path
 */
class MCLDFile : public llvm::sys::Path
{
public:
  bool isRecognized() const;
  const MCSectionELF *getELFSection(StringRef Section, unsigned Type,
                                    unsigned Flags, SectionKind Kind,
                                    unsigned EntrySize, StringRef Group);

  MCLDContext *m_pContext;

private:
  std::string m_AbsFilePath;
  std::string m_ArgName;
};

/** \class MCLDFileFactory
 *  \brief The allocator of MCLDFile
 *
 *  \see llvm::sys::Path
 */
class MCLDFileFactory : private LinearAllocator<MCLDFile, 64>
{
  MCLDFileFactory(const MCLDFileFactory& pCopy); /// NOT TO IMPLEMENT
  MCLDFileFactory& operator=(
                      const MCLDFileFactory& pCopy); /// NOT TO IMPLEMENT
public:
  MCLDFileFactory()
  { }

  ~MCLDFileFactory()
  { }
};

} // namespace of mcld

#endif

