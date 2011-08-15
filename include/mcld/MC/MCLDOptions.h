/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_OPTIONS_H
#define MCLD_OPTIONS_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Support/SearchDirs.h>
#include <mcld/Support/FileSystem.h>

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
  /// default bitcode
  bool hasDefaultBitcode() const;
  Input* defaultBitcode() const;
  void setDefaultBitcode(Input& pInput);

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

private:
  Input* m_pDefaultBitcode;
  std::string m_DefaultLDScript;
  sys::fs::Path m_Sysroot;
  SearchDirs m_SearchDirs;
  bool m_bTrace;
};

} // namespace of mcld

#endif

