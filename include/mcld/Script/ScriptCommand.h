//===- ScriptCommand.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SCRIPT_COMMAND_H
#define MCLD_SCRIPT_COMMAND_H

namespace mcld {

class Module;

/** \class ScriptCommand
 *  \brief This class defines the interfaces to a script command.
 */
class ScriptCommand
{
public:
  enum Kind {
    ENTRY,
    OUTPUT_FORMAT,
    GROUP,
    OUTPUT,
    SEARCH_DIR,
    OUTPUT_ARCH,
    ASSERT,
    ASSIGNMENT,
    SECTIONS,
    OUTPUT_SECT_DESC,
    INPUT_SECT_DESC
  };

protected:
  ScriptCommand(Kind pKind)
    : m_Kind(pKind)
  {}

public:
  virtual ~ScriptCommand() = 0;

  virtual void dump() const = 0;

  virtual void activate(Module&) = 0;

  Kind getKind() const { return m_Kind; }

private:
  Kind m_Kind;
};

} // namespace of mcld

#endif

