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
#include <mcld/Support/RealPath.h>
#include <mcld/MC/SearchDirs.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/MC/ZOption.h>

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
  GeneralOptions();
  ~GeneralOptions();

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

  void setPIE(bool pPIE = true)
  { m_bPIE = pPIE; }

  bool isPIE() const
  { return m_bPIE; }

  void setBgroup(bool pBgroup = false)
  { m_Bgroup = pBgroup; }

  bool Bgroup() const
  { return m_Bgroup; }

  bool hasEntry() const
  { return !m_Entry.empty(); }

  void setEntry(const std::string& pEntry)
  { m_Entry = pEntry; }

  const std::string& entry() const
  { return m_Entry; }

  ///  -----  the -z options  -----  ///
  void addZOption(const mcld::ZOption& pOption);

  bool hasCombReloc() const
  { return m_bCombReloc; }

  bool hasDefs() const
  { return m_bDefs; }

  bool hasStackSet() const
  { return (Unknown != m_ExecStack); }

  bool hasExecStack() const
  { return (YES == m_ExecStack); }

  bool hasInitFirst() const
  { return m_bInitFirst; }

  bool hasInterPose() const
  { return m_bInterPose; }

  bool hasLoadFltr() const
  { return m_bLoadFltr; }

  bool hasMulDefs() const
  { return m_bMulDefs; }

  bool hasNoCopyReloc() const
  { return m_bNoCopyReloc; }

  bool hasNoDefaultLib() const
  { return m_bNoDefaultLib; }

  bool hasNoDelete() const
  { return m_bNoDelete; }

  bool hasNoDLOpen() const
  { return m_bNoDLOpen; }

  bool hasNoDump() const
  { return m_bNoDump; }

  bool hasRelro() const
  { return m_bRelro; }

  bool hasNow() const
  { return m_bNow; }

  bool hasOrigin() const
  { return m_bOrigin; }

  uint64_t commPageSize() const
  { return m_CommPageSize; }

  uint64_t maxPageSize() const
  { return m_MaxPageSize; }

private:
  enum status {
    YES,
    NO,
    Unknown
  };

private:
  Input* m_pDefaultBitcode;
  std::string m_DefaultLDScript;
  sys::fs::RealPath m_Sysroot;
  SearchDirs m_SearchDirs;
  bool m_bTrace;
  bool m_bVerbose;
  bool m_Bsymbolic;
  bool m_Bgroup;
  bool m_bPIE;
  std::string m_Entry;
  /// the -z options
  bool m_bCombReloc;       // combreloc, nocombreloc
  bool m_bDefs;            // defs
  status m_ExecStack;      // execstack, noexecstack
  bool m_bInitFirst;       // initfirst
  bool m_bInterPose;       // interpose
  bool m_bLoadFltr;        // loadfltr
  bool m_bMulDefs;         // muldefs
  bool m_bNoCopyReloc;     // nocopyreloc
  bool m_bNoDefaultLib;    // nodefaultlib
  bool m_bNoDelete;        // nodelete
  bool m_bNoDLOpen;        // nodlopen
  bool m_bNoDump;          // nodump
  bool m_bRelro;           // relro, norelro
  bool m_bNow;             // lazy, now
  bool m_bOrigin;          // origin
  uint64_t m_CommPageSize; // common-page-size=value
  uint64_t m_MaxPageSize;  // max-page-size=value
};

} // namespace of mcld

#endif

