//===- PositionalOptions.h ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LDLITE_POSITIONAL_OPTIONS_H
#define MCLD_LDLITE_POSITIONAL_OPTIONS_H
#include <llvm/Support/CommandLine.h>
#include <mcld/Support/CommandLine.h>
#include <mcld/Support/Path.h>
#include <string>

namespace mcld {

class InputAction;
class LinkerConfig;
class LinkerScript;

/** \class PositionalOptions
 *
 *  The meaning of a positional option depends on its position and its related
 *  positions with the other positional options. There are four kinds of
 *  positional options:
 *   1. Inputs, object files, such as /tmp/XXXX.o
 *   2. Namespecs, short names of libraries. A namespec may refer to an archive
 *      or a shared library. For example, -lm.
 *   3. Attributes of inputs. Attributes describe inputs appears after them.
 *      For example, --as-needed and --whole-archive.
 *   4. Groups. A Group is a set of archives. Linkers repeatedly read archives
 *      in groups until there is no new undefined symbols.
 *   5. Definitions of symbols. --defsym option depends on
 */
class PositionalOptions
{
public:
  PositionalOptions();

  size_t numOfInputs() const;

  bool parse(std::vector<InputAction*>& pActions,
             const LinkerConfig& pConfig,
             const LinkerScript& pScript);

private:
  size_t numOfActions() const;

private:
  llvm::cl::list<mcld::sys::fs::Path>& m_InputObjectFiles;
  llvm::cl::list<std::string>& m_LinkerScript;
  llvm::cl::list<std::string>& m_NameSpecList;
  llvm::cl::list<bool>& m_WholeArchiveList;
  llvm::cl::list<bool>& m_NoWholeArchiveList;
  llvm::cl::list<bool>& m_AsNeededList;
  llvm::cl::list<bool>& m_NoAsNeededList;
  llvm::cl::list<bool>& m_AddNeededList;
  llvm::cl::list<bool>& m_NoAddNeededList;
  llvm::cl::list<bool>& m_BDynamicList;
  llvm::cl::list<bool>& m_BStaticList;
  llvm::cl::list<bool>& m_StartGroupList;
  llvm::cl::list<bool>& m_EndGroupList;
  llvm::cl::list<std::string>& m_DefSymList;

};

} // namespace of mcld

#endif

