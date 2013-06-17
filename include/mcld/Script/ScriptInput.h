//===- ScriptInput.h ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SCRIPT_INPUT_INTERFACE_H
#define MCLD_SCRIPT_INPUT_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Config/Config.h>
#include <mcld/Support/Allocators.h>
#include <vector>

namespace mcld
{

class StrToken;

/** \class ScriptInput
 *  \brief This class defines the interfaces to ScriptInput.
 */

class ScriptInput
{
public:
  typedef std::vector<StrToken*> InputList;
  typedef InputList::const_iterator const_iterator;
  typedef InputList::iterator iterator;
  typedef InputList::const_reference const_reference;
  typedef InputList::reference reference;

private:
  friend class Chunk<ScriptInput, MCLD_SYMBOLS_PER_INPUT>;
  ScriptInput();

public:
  ~ScriptInput();

  const_iterator  begin() const { return m_InputList.begin(); }
  iterator        begin()       { return m_InputList.begin(); }
  const_iterator  end()   const { return m_InputList.end(); }
  iterator        end()         { return m_InputList.end(); }

  const_reference front() const { return m_InputList.front(); }
  reference       front()       { return m_InputList.front(); }
  const_reference back()  const { return m_InputList.back(); }
  reference       back()        { return m_InputList.back(); }

  bool empty() const { return m_InputList.empty(); }

  void push_back(StrToken* pToken);

  /* factory methods */
  static ScriptInput* create();
  static void destroy(ScriptInput*& pScriptInput);
  static void clear();

private:
  InputList m_InputList;
};

} // namepsace of mcld

#endif

