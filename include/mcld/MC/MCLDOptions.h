//===- MCLDOptions.h ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_OPTIONS_H
#define MCLD_OPTIONS_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include "mcld/Support/RealPath.h"
#include "mcld/MC/SearchDirs.h"
#include "mcld/Support/FileSystem.h"

namespace mcld
{
class Input;

/** \class ScriptOptions
 *
 */
class ScriptOption
{
};

/** \class GeneralOptions
 *  \brief GeneralOptions collects the options that not be one of the
 *     - input files
 *     - attribute of input files
 *     - script options
 */
class GeneralOptions
{
public:
  /// default link script
  bool hasDefaultLDScript() const;
  const char* defaultLDScript() const;
  void setDefaultLDScript(const std::string& pFilename);

  /// sysroot
  const sys::fs::Path& sysroot() const
  { return m_Sysroot; }

  void setSysroot(const sys::fs::Path &pPath);

  /// search directory
  SearchDirs& directories()
  { return m_SearchDirs; }

  const SearchDirs& directories() const
  { return m_SearchDirs; }

  /// trace
  void setTrace(bool pEnableTrace = true)
  { m_bTrace = pEnableTrace; }

  bool trace() const
  { return m_bTrace; }

  void setVerbose(bool pVerbose = true)
  { m_bVerbose = pVerbose; }

  bool verbose() const
  { return m_bVerbose; }

  void setBsymbolic(bool pBsymbolic = false)
  { m_Bsymbolic = pBsymbolic; }

  bool Bsymbolic() const
  { return m_Bsymbolic; }

  bool hasEntry() const
  { return !m_Entry.empty(); }

  void setEntry(const std::string& pEntry)
  { m_Entry = pEntry; }

  const std::string& entry() const
  { return m_Entry; }

private:
  Input* m_pDefaultBitcode;
  std::string m_DefaultLDScript;
  sys::fs::RealPath m_Sysroot;
  SearchDirs m_SearchDirs;
  bool m_bTrace;
  bool m_bVerbose;
  bool m_Bsymbolic;
  std::string m_Entry;
};

} // namespace of mcld

#endif

