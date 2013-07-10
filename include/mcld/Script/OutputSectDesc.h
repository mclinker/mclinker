//===- OutputSectDesc.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SCRIPT_OUTPUT_SECTION_DESCRIPTION_INTERFACE_H
#define MCLD_SCRIPT_OUTPUT_SECTION_DESCRIPTION_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/ScriptCommand.h>
#include <vector>
#include <string>
#include <cassert>

namespace mcld
{

class RpnExpr;
class StringList;

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
    bool hasVMA() const { return m_pVMA != NULL; }
    const RpnExpr& vma() const {
      assert(hasVMA());
      return *m_pVMA;
    }

    Type type() const { return m_Type; }

    bool hasLMA() const { return m_pLMA != NULL; }
    const RpnExpr& lma() const {
      assert(hasLMA());
      return *m_pLMA;
    }

    bool hasAlign() const { return m_pAlign != NULL; }
    const RpnExpr& align() const {
      assert(hasAlign());
      return *m_pAlign;
    }

    bool hasSubAlign() const { return m_pSubAlign != NULL; }
    const RpnExpr& subAlign() const {
      assert(hasSubAlign());
      return *m_pSubAlign;
    }

    Constraint constraint() const { return m_Constraint; }

    RpnExpr* m_pVMA;
    Type m_Type;
    RpnExpr* m_pLMA;
    RpnExpr* m_pAlign;
    RpnExpr* m_pSubAlign;
    Constraint m_Constraint;
  };

  struct Epilog {
    bool hasRegion() const { return m_pRegion != NULL; }
    const std::string& region() const {
      assert(hasRegion());
      return *m_pRegion;
    }

    bool hasLMARegion() const { return m_pLMARegion != NULL; }
    const std::string& lmaRegion() const {
      assert(hasLMARegion());
      return *m_pLMARegion;
    }

    bool hasPhdrs() const { return m_pPhdrs != NULL; }
    const StringList& phdrs() const {
      assert(hasPhdrs());
      return *m_pPhdrs;
    }

    bool hasFillExp() const { return m_pFillExp != NULL; }
    const RpnExpr& fillExp() const {
      assert(hasFillExp());
      return *m_pFillExp;
    }

    const std::string* m_pRegion;
    const std::string* m_pLMARegion;
    StringList* m_pPhdrs;
    RpnExpr* m_pFillExp;
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

  void dump() const;

  static bool classof(const ScriptCommand* pCmd)
  {
    return pCmd->getKind() == ScriptCommand::OUTPUT_SECT_DESC;
  }

  void activate();

  void push_back(ScriptCommand* pCommand);

  void setEpilog(const Epilog& pEpilog);

  const Prolog& prolog() const { return m_Prolog; }

  const Epilog& epilog() const { return m_Epilog; }

private:
  OutputSectCmds m_OutputSectCmds;
  std::string m_Name;
  Prolog m_Prolog;
  Epilog m_Epilog;
};

} // namespace of mcld

#endif
