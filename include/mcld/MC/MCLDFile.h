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

#include <llvm/ADT/StringRef.h>
#include <mcld/MC/MCLDContext.h>
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
 *  MCLDFile is the storage of name, path and type
 *  It just refers to MCLDContext.
 *
 *  @see mcld::sys::fs::Path MCLDContext
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
  MCLDFile(llvm::StringRef pName,
           const sys::fs::Path& pPath,
           unsigned int pType = Unknown);

  ~MCLDFile();

  // -----  modifiers  ----- //
  void setType(Type pType)
  { m_Type = pType; }

  // -----  observers  ----- //
  bool isRecognized() const
  { return (m_Type != Unknown); }

  unsigned int type() const
  { return m_Type; }

  llvm::StringRef name() const;

  const sys::fs::Path& path() const
  { return m_Path; }

private:
  sys::fs::Path m_Path;
  std::string m_InputName;
  MCLDContext   *m_pContext;
  unsigned int m_Type;
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
  MCLDFile* produce(llvm::StringRef pName,
                    const sys::fs::Path& pPath,
                    unsigned int pType = MCLDFile::Unknown);
  
  MCLDFile* produce();
  // -----  iterators  ----- //
  iterator begin()             { return Alloc::begin(); }
  iterator end()               { return Alloc::end(); }
  const_iterator begin() const { return Alloc::begin(); }
  const_iterator end() const   { return Alloc::end(); }

  // -----  observers  ----- //
  unsigned int size() const
  { return m_NumCreatedFiles; }

  bool empty() const
  { return Alloc::empty(); }

private:
  unsigned int m_NumCreatedFiles;
};

} // namespace of mcld

#endif

