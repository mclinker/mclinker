//===- WildcardPattern.h --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SCRIPT_WILDCARDPATTERN_H
#define MCLD_SCRIPT_WILDCARDPATTERN_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/StrToken.h>
#include <mcld/Support/Allocators.h>
#include <mcld/Config/Config.h>

namespace mcld
{

/** \class WildcardPattern
 *  \brief This class defines the interfaces to Input Section Wildcard Patterns
 */

class WildcardPattern : public StrToken
{
public:
  enum SortPolicy {
    SORT_NONE,
    SORT_BY_NAME,
    SORT_BY_ALIGNMENT,
    SORT_BY_NAME_ALIGNMENT,
    SORT_BY_ALIGNMENT_NAME,
    SORT_BY_INIT_PRIORITY
  };

private:
  friend class Chunk<WildcardPattern, MCLD_SYMBOLS_PER_INPUT>;
  WildcardPattern();
  WildcardPattern(const std::string& pPattern, SortPolicy pPolicy);

public:
  ~WildcardPattern();

  SortPolicy sortPolicy() const { return m_SortPolicy; }

  static bool classof(const StrToken* pToken)
  {
    return pToken->kind() == StrToken::Wildcard;
  }

  /* factory method */
  static WildcardPattern* create(const std::string& pPattern,
                                 SortPolicy pPolicy);
  static void destroy(WildcardPattern*& pToken);
  static void clear();

private:
  SortPolicy m_SortPolicy;
};

} // namepsace of mcld

#endif
