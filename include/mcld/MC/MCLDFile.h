/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCLD_LDFILE_H
#define MCLD_LDFILE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/MC/MCLDContext.h>
#include <llvm/Support/MemoryBuffer.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/ADT/Allocators.h>
#include <string>

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
class MCLDFile
{
public:
  enum Type {
    Archive,
    Object,
    Script,
    Unknown
  };

public:
  MCLDFile();
  ~MCLDFile();

  bool isRecognized() const
  { return (m_Type != Unknown); }

  Type type() const
  { return m_Type; }

private:
  sys::fs::Path m_Path;
  std::string   m_InputName;
  MCLDContext   *m_pContext;
  Type          m_Type;
};

/** \class MCLDFileFactory
 *  \brief MCLDFileFactory controls the production and destruction of
 *  MCLDFiles.
 *
 *  All MCLDFiles created by MCLDFileFactory are guaranteed to be destructed
 *  while MCLDFileFactory is destructed.
 *
 *  \see llvm::sys::Path
 */
class MCLDFileFactory : private LinearAllocator<MCLDFile, 64>
{
private:
  typedef LinearAllocator<MCLDFile, 64> Alloc;

public:
  typedef Alloc::iterator               iterator;
  typedef Alloc::const_iterator         const_iterator;

public:
  MCLDFileFactory()
  : m_NumCreatedFiles(0)
  { }

  virtual ~MCLDFileFactory()
  { Alloc::clear(); }

  // -----  production  ----- //
  MCLDFile* produce() {
    MCLDFile* result = Alloc::allocate();
    Alloc::construct(result);
    ++m_NumCreatedFiles;
    return result;
  }
  
  // -----  iterators  ----- //
  iterator begin()             { return Alloc::begin(); }
  iterator end()               { return Alloc::end(); }
  const_iterator begin() const { return Alloc::begin(); }
  const_iterator end() const   { return Alloc::end(); }

private:
  unsigned int m_NumCreatedFiles;
};

} // namespace of mcld

#endif

