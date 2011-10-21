//===- MCLDFile.h ---------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LDFILE_H
#define MCLD_LDFILE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "llvm/ADT/StringRef.h"
#include "mcld/MC/MCLDContext.h"
#include "mcld/Support/Path.h"
#include "mcld/Support/FileSystem.h"
#include "mcld/Support/GCFactory.h"
#include "mcld/ADT/Uncopyable.h"
#include "mcld/Support/MemoryArea.h"
#include <string>

#include "mcld/MC/MCLDContext.h"

namespace mcld
{
class MCContext;
class MCLDContext;
class MemoryArea;

/** \class MCLDFile
 *  \brief MCLDFile represents the file being linked or produced.
 *
 *  MCLDFile is the storage of name, path and type
 *  A MCLDFile just refers to MCLDContext, not owns it.
 *
 *  @see mcld::sys::fs::Path MCLDContext
 */
class MCLDFile : private Uncopyable
{
public:
  enum Type {
    Unknown,
    Object,
    Exec,
    DynObj,
    CoreFile,
    Script,
    Archive
  };

public:
  MCLDFile();
  MCLDFile(llvm::StringRef pName);
  MCLDFile(llvm::StringRef pName,
           const sys::fs::Path& pPath,
           unsigned int pType = Unknown);

  virtual ~MCLDFile();

  // -----  modifiers  ----- //
  void setType(unsigned int pType)
  { m_Type = pType; }

  void setContext(MCLDContext* pContext)
  { m_pContext = pContext; }

  void setPath(const sys::fs::Path& pPath)
  { m_Path = pPath; }

  void setMemArea(MemoryArea* pMemArea)
  { m_pMemArea = pMemArea; }

  // -----  observers  ----- //
  unsigned int type() const
  { return m_Type; }

  const std::string& name() const;

  const sys::fs::Path& path() const
  { return m_Path; }

  bool hasContext() const
  { return (0 == m_pContext); }

  MCLDContext* context()
  { return m_pContext; }

  const MCLDContext* context() const
  { return m_pContext; }

  bool hasMemArea() const
  { return (0 == m_pMemArea); }

  MemoryArea* memArea()
  { return m_pMemArea; }

  const MemoryArea* memArea() const
  { return m_pMemArea; }

protected:
  sys::fs::Path m_Path;
  std::string m_InputName;
  unsigned int m_Type;
  MCLDContext *m_pContext;
  MemoryArea* m_pMemArea;

};

/** \class MCLDFileFactory
 *  \brief MCLDFileFactory controls the production and destruction of
 *  MCLDFiles.
 *
 *  All MCLDFiles created by MCLDFileFactory are guaranteed to be destructed
 *  while MCLDFileFactory is destructed.
 *
 *  MCLDFileFactory also provides the MCLCContextFactory to MCLDFile.
 *  MCLDFile is responsed for the life of MCLDContext, therefore, the best
 *  idea is let MCLDFile control the life of MCLDContext. Since SectLinker
 *  has the need to count the number of MCLDContext, we give a central factory
 *  for MCLDContext.
 *
 *  \see llvm::sys::Path
 */
template<size_t NUM>
class MCLDFileFactory : public GCFactory<MCLDFile, NUM>
{
public:
  typedef GCFactory<MCLDFile, NUM> Alloc;

public:
  // -----  production  ----- //
  MCLDFile* produce(llvm::StringRef pName,
                    const sys::fs::Path& pPath,
                    unsigned int pType = MCLDFile::Unknown);
  
  MCLDFile* produce();
};

} // namespace of mcld

//===----------------------------------------------------------------------===//
// MCLDFileFactory
template<size_t NUM>
mcld::MCLDFile* mcld::MCLDFileFactory<NUM>::produce(llvm::StringRef pName,
                                   const mcld::sys::fs::Path& pPath,
                                   unsigned int pType)
{
    mcld::MCLDFile* result = Alloc::allocate();
    new (result) mcld::MCLDFile(pName, pPath, pType);
    return result;
}

template<size_t NUM>
mcld::MCLDFile* mcld::MCLDFileFactory<NUM>::produce()
{
    mcld::MCLDFile* result = Alloc::allocate();
    new (result) mcld::MCLDFile();
    return result;
}

#endif

