//===- PositionDependentOption.h ------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_POSITIONDEPENDENTOPTION_H
#define MCLD_POSITIONDEPENDENTOPTION_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <vector>

namespace mcld
{

  /** \class PositionDependentOption
   *  \brief PositionDependentOptions converts LLVM options into MCLDInfo
   */
  class PositionDependentOption
  {
  public:
    enum Type {
      BITCODE,
      NAMESPEC,
      INPUT_FILE,
      START_GROUP,
      END_GROUP,
      WHOLE_ARCHIVE,
      NO_WHOLE_ARCHIVE,
      AS_NEEDED,
      NO_AS_NEEDED,
      ADD_NEEDED,
      NO_ADD_NEEDED,
      BDYNAMIC,
      BSTATIC
    };

  protected:
    PositionDependentOption(unsigned pPosition, Type pType)
      : m_Type(pType),
        m_Position(pPosition) {}

  public:
    inline const Type& type() const
    { return m_Type; }

    inline unsigned position() const
    { return m_Position; }

  private:
    Type m_Type;
    unsigned m_Position;
  };

  typedef std::vector<PositionDependentOption*> PositionDependentOptions;
} // namespace of mcld

#endif

