//===- MCLDFile.h ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// MCLDFile represents a file, the content of the file is stored in LDContext.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LD_FILE_H
#define MCLD_LD_FILE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/ADT/Uncopyable.h>

namespace mcld {

class LDContext;

/** \class MCLDFile
 *  \brief MCLDFile represents the file being linked or produced.
 *
 *  MCLDFile is the storage of name, path and type
 *  A MCLDFile just refers to LDContext, not owns it.
 *
 *  @see mcld::sys::fs::Path LDContext
 */
class MCLDFile : private Uncopyable
{
public:
  MCLDFile();

  virtual ~MCLDFile();

  void setContext(LDContext* pContext)
  { m_pContext = pContext; }

  bool hasContext() const
  { return (NULL != m_pContext); }

  const LDContext* context() const { return m_pContext; }
  LDContext*       context()       { return m_pContext; }

protected:
  LDContext *m_pContext;
};

} // namespace of mcld

#endif

