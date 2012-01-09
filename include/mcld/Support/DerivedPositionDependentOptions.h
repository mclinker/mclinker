//===- DerivedPositionDependentOptions.h ----------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_DERIVEDPOSITIONDEPENDENTOPTIONS_H
#define MCLD_DERIVEDPOSITIONDEPENDENTOPTIONS_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <string>

#include "mcld/Support/RealPath.h"
#include "mcld/Support/PositionDependentOption.h"

namespace mcld
{

  /** \class DerivedPositionDependentOptions
   *  \brief This file contains the declarations of classes derived from PositionDependentOption.
   */

  class FileOption : public PositionDependentOption {
  private:
    sys::fs::RealPath m_Path;

  protected:
    FileOption(unsigned pPosition, Type pType, const sys::fs::Path &pPath)
      : PositionDependentOption(pPosition, pType)
    { m_Path.assign(pPath); }

  public:
    inline const sys::fs::Path *path() const { return &m_Path; }
  };

  class NamespecOption : public PositionDependentOption {
  private:
    std::string m_pNamespec;

  public:
    NamespecOption(unsigned pPosition, const std::string &pNamespec)
      : PositionDependentOption(pPosition, PositionDependentOption::NAMESPEC),
        m_pNamespec(pNamespec) { }

    inline const std::string &namespec() const { return m_pNamespec; }
  };

  class BitcodeOption : public FileOption {
  public:
    BitcodeOption(unsigned pPosition, const sys::fs::Path &pPath)
      : FileOption(pPosition, PositionDependentOption::BITCODE, pPath) { }
  };

  class StartGroupOption : public PositionDependentOption {
  public:
    StartGroupOption(unsigned pPosition)
      : PositionDependentOption(pPosition,
                                PositionDependentOption::START_GROUP) { }
  };

  class EndGroupOption : public PositionDependentOption {
  public:
    EndGroupOption(unsigned pPosition)
      : PositionDependentOption(pPosition,
                                PositionDependentOption::END_GROUP) { }
  };

  class InputFileOption : public FileOption {
  public:
    InputFileOption(unsigned pPosition, const sys::fs::Path &pPath)
      : FileOption(pPosition, PositionDependentOption::INPUT_FILE, pPath) { }
  };

  class WholeArchiveOption : public PositionDependentOption {
  public:
    WholeArchiveOption(unsigned pPosition)
      : PositionDependentOption(pPosition,
                                PositionDependentOption::WHOLE_ARCHIVE) { }
  };

  class NoWholeArchiveOption : public PositionDependentOption {
  public:
    NoWholeArchiveOption(unsigned pPosition)
      : PositionDependentOption(pPosition,
                                PositionDependentOption::NO_WHOLE_ARCHIVE) { }
  };

  class AsNeededOption : public PositionDependentOption {
  public:
    AsNeededOption(unsigned pPosition)
      : PositionDependentOption(pPosition,
                                PositionDependentOption::AS_NEEDED) { }
  };

  class NoAsNeededOption : public PositionDependentOption {
  public:
    NoAsNeededOption(unsigned pPosition)
      : PositionDependentOption(pPosition,
                                PositionDependentOption::NO_AS_NEEDED) { }
  };

  class AddNeededOption : public PositionDependentOption {
  public:
    AddNeededOption(unsigned pPosition)
      : PositionDependentOption(pPosition,
                                PositionDependentOption::ADD_NEEDED) { }
  };

  class NoAddNeededOption : public PositionDependentOption {
  public:
    NoAddNeededOption(unsigned pPosition)
      : PositionDependentOption(pPosition,
                                PositionDependentOption::NO_ADD_NEEDED) { }
  };

  class BDynamicOption : public PositionDependentOption {
  public:
    BDynamicOption(unsigned pPosition)
      : PositionDependentOption(pPosition,
                                PositionDependentOption::BDYNAMIC) { }
  };

  class BStaticOption : public PositionDependentOption {
  public:
    BStaticOption(unsigned pPosition)
      : PositionDependentOption(pPosition,
                                PositionDependentOption::BSTATIC) { }
  };

} // namespace of mcld

#endif

