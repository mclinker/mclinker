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

#include <mcld/Support/Path.h>
#include <vector>

namespace mcld
{

class Path;

class ScriptInput
{
public:
  class Node {
  public:
    Node(const std::string& pPath, bool pAsNeeded)
      : m_Path(pPath), m_bAsNeeded(pAsNeeded)
    {}
    ~Node()
    {}

    const sys::fs::Path& path() const { return m_Path; }

    bool asNeeded() const { return m_bAsNeeded; }
  private:
    sys::fs::Path m_Path;
    bool m_bAsNeeded;
  };

  typedef std::vector<Node> InputList;
  typedef InputList::const_iterator const_iterator;
  typedef InputList::iterator iterator;
  typedef InputList::const_reference const_reference;
  typedef InputList::reference reference;

public:
  ScriptInput();
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

  void append(const std::string& pPath);

  /// AS_NEEDED(file, file, ...)
  /// AS_NEEDED(file file ...)
  void setAsNeeded(bool pEnable = true);

private:
  InputList m_InputList;
  bool m_bAsNeeded;
};

} // namepsace of mcld

#endif

