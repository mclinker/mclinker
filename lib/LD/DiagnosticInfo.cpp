//===- DiagnosticInfo.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/DiagnosticInfo.h>
#include <mcld/LD/DiagnosticEngine.h>
#include <llvm/ADT/StringRef.h>

using namespace mcld;

namespace {

struct DiagStaticInfo
{
  uint16_t ID;
  DiagnosticEngine::Level Class;
  uint16_t DescriptionLen;
  const char* DescriptionStr;

  llvm::StringRef getDescription() const
  { return llvm::StringRef(DescriptionStr, DescriptionLen); }

  bool operator<(const DiagStaticInfo& pRHS)
  { return (ID < pRHS.ID); }
};

} // namespace anonymous

static const DiagStaticInfo DiagCommonInfo = {
#define DIAG(ENUM, CLASS, ADDRDESC, LOCDESC) \
  {ENUM, CLASS, STR_SIZE(ADDRDESC, uint16_t), ADDRDESC },
#include "mcld/LD/DiagCommonKinds.inc"
#include "mcld/LD/DiagSymbolResolutions.inc"
  { 0, 0, 0, 0}
};

static const unsigned int DiagCommonInfoSize =
  sizeof(DiagCommonInfo)/sizeof(DiagCommonInfo[0])-1;

static const DiagStaticInfo DiagLoCInfo = {
#define DIAG(ENUM, CLASS, ADDRDESC, LOCDESC) \
  {ENUM, CLASS, STR_SIZE(LOCDESC, uint16_t), LOCDESC },
#include "mcld/LD/DiagSymbolResolutions.inc"
  { 0, 0, 0, 0}
};

static const unsigned int DiagLocInfoSize =
  sizeof(DiagLocInfo)/sizeof(DiagLocInfo[0])-1;



