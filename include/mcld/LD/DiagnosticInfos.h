//===- DiagnosticInfo.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LD_DIAGNOSTICINFORMATION_H
#define MCLD_LD_DIAGNOSTICINFORMATION_H
#include <llvm/ADT/StringRef.h>

namespace mcld {

namespace diag {
  enum ID {
#define DIAG(ENUM, CLASS, ADDRMSG, LINEMSG) ENUM,
#include "mcld/LD/DiagAttribute.inc"
#include "mcld/LD/DiagCommonKinds.inc"
#include "mcld/LD/DiagReaders.inc"
#include "mcld/LD/DiagSymbolResolutions.inc"
#include "mcld/LD/DiagRelocations.inc"
#include "mcld/LD/DiagLayouts.inc"
#include "mcld/LD/DiagGOTPLT.inc"
#include "mcld/LD/DiagLDScript.inc"
#undef DIAG
    NUM_OF_BUILDIN_DIAGNOSTIC_INFO
  };
} // namespace of diag

class LinkerConfig;
class DiagnosticEngine;

/** \class DiagnosticInfos
 *  \brief DiagnosticInfos caches run-time information of DiagnosticInfo.
 */
class DiagnosticInfos
{
public:
  DiagnosticInfos(const LinkerConfig& pConfig);

  ~DiagnosticInfos();

  llvm::StringRef getDescription(unsigned int pID, bool pLoC) const;

  bool process(DiagnosticEngine& pEngine) const;

private:
  const LinkerConfig& m_Config;
};

} // namespace of mcld

#endif

