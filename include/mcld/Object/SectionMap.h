//===- SectionMap.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SECTION_MAP_H
#define MCLD_SECTION_MAP_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/OutputSectDesc.h>
#include <mcld/Script/InputSectDesc.h>
#include <mcld/Script/Assignment.h>
#include <llvm/Support/DataTypes.h>
#include <vector>
#include <string>

namespace mcld {

class LDSection;

/** \class SectionMap
 *  \brief descirbe how to map input sections into output sections
 */
class SectionMap
{
public:
  class Input {
  public:
    Input(const std::string& pName);

    InputSectDesc::KeepPolicy policy() const { return m_Policy; }

    const InputSectDesc::Spec& spec() const { return m_Spec; }

    const LDSection* getSection() const { return m_pSection; }
    LDSection*       getSection()       { return m_pSection; }

  private:
    InputSectDesc::KeepPolicy m_Policy;
    InputSectDesc::Spec m_Spec;
    LDSection* m_pSection;
  };

  class Output {
  public:
    typedef std::vector<Input*> InputList;
    typedef InputList::const_iterator const_iterator;
    typedef InputList::iterator iterator;
    typedef InputList::const_reference const_reference;
    typedef InputList::reference reference;

    Output(const std::string& pName);

    const std::string& name() const { return m_Name; }

    const OutputSectDesc::Prolog& prolog() const { return m_Prolog; }

    const OutputSectDesc::Epilog& epilog() const { return m_Epilog; }

    size_t order() const { return m_Order; }

    bool hasContent() const;

    const LDSection* getSection() const { return m_pSection; }
    LDSection*       getSection()       { return m_pSection; }

    void setSection(LDSection* pSection) { m_pSection = pSection; }

    const_iterator begin() const { return m_InputList.begin(); }
    iterator       begin()       { return m_InputList.begin(); }
    const_iterator end  () const { return m_InputList.end(); }
    iterator       end  ()       { return m_InputList.end(); }

    const_reference front() const { return m_InputList.front(); }
    reference       front()       { return m_InputList.front(); }
    const_reference back () const { return m_InputList.back(); }
    reference       back ()       { return m_InputList.back(); }

    size_t size() const { return m_InputList.size(); }

    bool empty() const { return m_InputList.empty(); }

    void append(Input* pInput) { m_InputList.push_back(pInput); }

  private:
    std::string m_Name;
    OutputSectDesc::Prolog m_Prolog;
    OutputSectDesc::Epilog m_Epilog;
    LDSection* m_pSection;
    size_t m_Order;
    InputList m_InputList;
  };

  typedef std::pair<const Output*, const Input*> const_mapping;
  typedef std::pair<Output*, Input*> mapping;

  typedef std::vector<Output*> OutputDescList;
  typedef OutputDescList::const_iterator const_iterator;
  typedef OutputDescList::iterator iterator;
  typedef OutputDescList::const_reference const_reference;
  typedef OutputDescList::reference reference;

public:
  ~SectionMap();

  const_mapping find(const std::string& pInputSection) const;
  mapping       find(const std::string& pInputSection);

  std::pair<mapping, bool>
  insert(const std::string& pInputSection, const std::string& pOutputSection);

  bool   empty() const { return m_OutputDescList.empty(); }
  size_t size () const { return m_OutputDescList.size(); }

  const_iterator begin() const { return m_OutputDescList.begin(); }
  iterator       begin()       { return m_OutputDescList.begin(); }
  const_iterator end  () const { return m_OutputDescList.end(); }
  iterator       end  ()       { return m_OutputDescList.end(); }

  const_reference front() const { return m_OutputDescList.front(); }
  reference       front()       { return m_OutputDescList.front(); }
  const_reference back () const { return m_OutputDescList.back(); }
  reference       back ()       { return m_OutputDescList.back(); }

private:
  bool matched(const Input& pInput, const std::string& pString) const;

private:
  OutputDescList m_OutputDescList;
};

} // namespace of mcld

#endif

