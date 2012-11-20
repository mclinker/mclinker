//===- GeneralOptions.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_GENERAL_OPTIONS_H
#define MCLD_GENERAL_OPTIONS_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <string>
#include <mcld/Support/RealPath.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/MC/SearchDirs.h>
#include <mcld/MC/ZOption.h>

namespace mcld {

class Input;

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
  const sys::fs::Path& sysroot() const;

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

  void setBsymbolic(bool pBsymbolic = true)
  { m_Bsymbolic = pBsymbolic; }

  bool Bsymbolic() const
  { return m_Bsymbolic; }

  void setPIE(bool pPIE = true)
  { m_bPIE = pPIE; }

  bool isPIE() const
  { return m_bPIE; }

  void setBgroup(bool pBgroup = true)
  { m_Bgroup = pBgroup; }

  bool Bgroup() const
  { return m_Bgroup; }

  bool hasEntry() const
  { return !m_Entry.empty(); }

  void setEntry(const std::string& pEntry)
  { m_Entry = pEntry; }

  const std::string& entry() const
  { return m_Entry; }

  void setDyld(const std::string& pDyld)
  { m_Dyld = pDyld; }

  const std::string& dyld() const
  { return m_Dyld; }

  bool hasDyld() const
  { return !m_Dyld.empty(); }

  void setSOName(const std::string& pName);

  const std::string& soname() const
  { return m_SOName; }

  void setAllowShlibUndefined(bool pEnabled = true)
  { m_bAllowShlibUndefined = pEnabled; }

  bool isAllowShlibUndefined() const
  { return m_bAllowShlibUndefined; }

  void setVerbose(int8_t pVerbose = -1)
  { m_Verbose = pVerbose; }

  int8_t verbose() const
  { return m_Verbose; }

  void setMaxErrorNum(int16_t pNum)
  { m_MaxErrorNum = pNum; }

  int16_t maxErrorNum() const
  { return m_MaxErrorNum; }

  void setMaxWarnNum(int16_t pNum)
  { m_MaxWarnNum = pNum; }

  int16_t maxWarnNum() const
  { return m_MaxWarnNum; }

  void setColor(bool pEnabled = true)
  { m_bColor = pEnabled; }

  bool color() const
  { return m_bColor; }

  void setNoUndefined(bool pEnable = true)
  { m_bNoUndefined = pEnable; }

  void setMulDefs(bool pEnable = true)
  { m_bMulDefs = pEnable; }

  void setEhFrameHdr(bool pEnable = true)
  { m_bCreateEhFrameHdr = pEnable; }

  ///  -----  the -z options  -----  ///
  void addZOption(const mcld::ZOption& pOption);

  bool hasCombReloc() const
  { return m_bCombReloc; }

  bool isNoUndefined() const
  { return m_bNoUndefined; }

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

  bool hasEhFrameHdr() const
  { return m_bCreateEhFrameHdr; }

  // -n, --nmagic
  void setNMagic(bool pMagic = true)
  { m_NMagic = pMagic; }

  bool nnagic() const
  { return m_NMagic; }

  // -N, --omagic
  void setOMagic(bool pMagic = true)
  { m_OMagic = pMagic; }

  bool omagic() const
  { return m_OMagic; }

  // -S, --strip-debug
  void setStripDebug(bool pStripDebug = true)
  { m_StripDebug = pStripDebug; }

  bool stripDebug() const
  { return m_StripDebug; }

  // -E, --export-dynamic
  void setExportDynamic(bool pExportDynamic = true)
  { m_ExportDynamic = pExportDynamic; }

  bool exportDynamic() const
  { return m_ExportDynamic; }

  // --warn-shared-textrel
  void setWarnSharedTextrel(bool pWarnSharedTextrel = true)
  { m_WarnSharedTextrel = pWarnSharedTextrel; }

  bool warnSharedTextrel() const
  { return m_WarnSharedTextrel; }

private:
  enum status {
    YES,
    NO,
    Unknown
  };

private:
  Input* m_pDefaultBitcode;
  std::string m_DefaultLDScript;
  SearchDirs m_SearchDirs;
  std::string m_Entry;
  std::string m_Dyld;
  std::string m_SOName;
  int8_t m_Verbose;            // --verbose[=0,1,2]
  uint16_t m_MaxErrorNum;      // --error-limit=N
  uint16_t m_MaxWarnNum;       // --warning-limit=N
  status m_ExecStack;          // execstack, noexecstack
  uint64_t m_CommPageSize;     // common-page-size=value
  uint64_t m_MaxPageSize;      // max-page-size=value
  bool m_bCombReloc     : 1;   // combreloc, nocombreloc
  bool m_bNoUndefined   : 1;   // defs, --no-undefined
  bool m_bInitFirst     : 1;   // initfirst
  bool m_bInterPose     : 1;   // interpose
  bool m_bLoadFltr      : 1;   // loadfltr
  bool m_bMulDefs       : 1;   // muldefs
  bool m_bNoCopyReloc   : 1;   // nocopyreloc
  bool m_bNoDefaultLib  : 1;   // nodefaultlib
  bool m_bNoDelete      : 1;   // nodelete
  bool m_bNoDLOpen      : 1;   // nodlopen
  bool m_bNoDump        : 1;   // nodump
  bool m_bRelro         : 1;   // relro, norelro
  bool m_bNow           : 1;   // lazy, now
  bool m_bOrigin        : 1;   // origin
  bool m_bTrace         : 1;   // --trace
  bool m_Bsymbolic      : 1;   // --Bsymbolic
  bool m_Bgroup         : 1;
  bool m_bPIE           : 1;
  bool m_bColor         : 1;   // --color[=true,false,auto]
  bool m_bAllowShlibUndefined : 1; // --[no-]allow-shlib-undefined and
  bool m_bCreateEhFrameHdr : 1;    // --eh-frame-hdr
  bool m_NMagic; // -n, --nmagic
  bool m_OMagic; // -N, --omagic
  bool m_StripDebug; // -S, --strip-debug
  bool m_ExportDynamic; //-E, --export-dynamic
  bool m_WarnSharedTextrel; // --warn-shared-textrel
};

} // namespace of mcld

#endif

