//===- HexagonPLT.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TARGET_HEXAGON_PLT_H
#define MCLD_TARGET_HEXAGON_PLT_H

#include "HexagonGOT.h"
#include <mcld/Target/PLT.h>

namespace {

const uint8_t hexagon_dyn_plt0[] = {
  0
};

const uint8_t hexagon_exec_plt0[] = {
  0
};

} // anonymous namespace

namespace mcld {

class GOTEntry;
class LinkerConfig;

//===----------------------------------------------------------------------===//
// HexagonPLT Entry
//===----------------------------------------------------------------------===//
class HexagonDynPLT0 : public PLT::Entry<sizeof(hexagon_dyn_plt0)>
{
public:
  HexagonDynPLT0(SectionData& pParent);
};

class HexagonExecPLT0 : public PLT::Entry<sizeof(hexagon_exec_plt0)>
{
public:
  HexagonExecPLT0(SectionData& pParent);
};

//===----------------------------------------------------------------------===//
// HexagonPLT
//===----------------------------------------------------------------------===//
/** \class HexagonPLT
 *  \brief Hexagon Procedure Linkage Table
 */
class HexagonPLT : public PLT
{
public:
  HexagonPLT(LDSection& pSection,
             HexagonGOT& pGOTPLT,
             const LinkerConfig& pConfig);
  ~HexagonPLT();

  // finalizeSectionSize - set LDSection size
  void finalizeSectionSize();

  void reserveEntry(size_t pNum = 1) ;

private:
  PLTEntryBase* getPLT0() const;

private:
  HexagonGOT& m_GOT;

  // the last consumed entry.
  SectionData::iterator m_Last;

  const uint8_t *m_PLT0;
  unsigned int m_PLT0Size;

  const LinkerConfig& m_Config;
};

} // namespace of mcld

#endif

