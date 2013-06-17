//===- OutputSectDesc.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_OUTPUT_SECTION_DESCRIPTION_INTERFACE_H
#define MCLD_OUTPUT_SECTION_DESCRIPTION_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/ScriptCommand.h>
#include <vector>
#include <string>

namespace mcld
{

class RpnExpr;
class ScriptInput;

/** \class OutputSectDesc
 *  \brief This class defines the interfaces to output section description.
 */

class OutputSectDesc : public ScriptCommand
{
public:
  enum Type {
    LOAD, // ALLOC
    NOLOAD,
    DSECT,
    COPY,
    INFO,
    OVERLAY
  };

  enum Constraint {
    NO_CONSTRAINT,
    ONLY_IF_RO,
    ONLY_IF_RW
  };

  struct Prolog {
    RpnExpr* vma;
    Type type;
    RpnExpr* lma;
    RpnExpr* align;
    RpnExpr* sub_align;
    Constraint constraint;
  };

  struct Epilog {
    const std::string* region;
    const std::string* lma_region;
    ScriptInput* phdrs;
    RpnExpr* fill_exp;
  };

  typedef std::vector<ScriptCommand*> OutputSectCmds;
  typedef OutputSectCmds::const_iterator const_iterator;
  typedef OutputSectCmds::iterator iterator;
  typedef OutputSectCmds::const_reference const_reference;
  typedef OutputSectCmds::reference reference;

public:
  OutputSectDesc(const std::string& pName, const Prolog& pProlog);
  ~OutputSectDesc();

  const_iterator  begin() const { return m_OutputSectCmds.begin(); }
  iterator        begin()       { return m_OutputSectCmds.begin(); }
  const_iterator  end()   const { return m_OutputSectCmds.end(); }
  iterator        end()         { return m_OutputSectCmds.end(); }

  const_reference front() const { return m_OutputSectCmds.front(); }
  reference       front()       { return m_OutputSectCmds.front(); }
  const_reference back()  const { return m_OutputSectCmds.back(); }
  reference       back()        { return m_OutputSectCmds.back(); }

  const std::string& name() const { return m_Name; }

  size_t size() const { return m_OutputSectCmds.size(); }

  bool empty() const { return m_OutputSectCmds.empty(); }

  bool hasVMA() const;

  RpnExpr& vma();

  const RpnExpr& vma() const;

  Type type() const;

  bool hasLMA() const;

  RpnExpr& lma();

  const RpnExpr& lma() const;

  bool hasAlign() const;

  RpnExpr& align();

  const RpnExpr& align() const;

  bool hasSubAlign() const;

  RpnExpr& subAlign();

  const RpnExpr& subAlign() const;

  Constraint constraint() const;

  bool hasRegion() const;

  const std::string& region() const;

  bool hasLMARegion() const;

  const std::string& lmaRegion() const;

  bool hasPhdrs() const;

  ScriptInput& phdrs();

  const ScriptInput& phdrs() const;

  bool hasFillExp() const;

  RpnExpr& fillExp();

  const RpnExpr& fillExp() const;

  void dump() const;

  static bool classof(const ScriptCommand* pCmd)
  {
    return pCmd->getKind() == ScriptCommand::OutputSectDesc;
  }

  void activate();

  void push_back(ScriptCommand* pCommand);

  void setEpilog(const Epilog& pEpilog);

private:
  OutputSectCmds m_OutputSectCmds;
  std::string m_Name;
  Prolog m_Prolog;
  Epilog m_Epilog;
};

} // namespace of mcld

#endif
