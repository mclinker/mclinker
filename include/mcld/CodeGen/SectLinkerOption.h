//===- SectLinkerOption.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SECTLINKERDATA_H
#define MCLD_SECTLINKERDATA_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/MC/MCLDInfo.h>
#include <mcld/Support/PositionDependentOption.h>

#include <string>

namespace mcld
{
  class PositionDependentOption;

  /** \class SectLinkerOption
   *  \brief This file collects inputs to linker.
   */
  class SectLinkerOption
  {
  public:
    // Constructor.
    SectLinkerOption(MCLDInfo &pLDInfo);

    // -----  Position-dependent Options  ----- //
    inline void appendOption(PositionDependentOption *pOption)
    { m_PosDepOptions.push_back(pOption); }

    inline void prependOption(PositionDependentOption *pOption)
    { m_PosDepOptions.insert(m_PosDepOptions.begin(), pOption); }

    inline const PositionDependentOptions &pos_dep_options() const
    { return m_PosDepOptions; }
    inline PositionDependentOptions &pos_dep_options()
    { return m_PosDepOptions; }

    inline const MCLDInfo &info() const { return *m_pLDInfo; }
    inline MCLDInfo &info() { return *m_pLDInfo; }

    ~SectLinkerOption();

  private:
    MCLDInfo *m_pLDInfo;
    PositionDependentOptions m_PosDepOptions;
  };

} // namespace of mcld

#endif
