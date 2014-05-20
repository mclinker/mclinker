//===- PositionalOptions.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/PositionalOptions.h>
#include <mcld/LinkerConfig.h>
#include <mcld/LinkerScript.h>
#include <mcld/MC/InputAction.h>
#include <mcld/MC/CommandAction.h>
#include <mcld/MC/FileAction.h>
#include <mcld/Support/MsgHandling.h>

namespace {

//===----------------------------------------------------------------------===//
// Normal input files
//===----------------------------------------------------------------------===//
llvm::cl::list<mcld::sys::fs::Path> ArgInputObjectFiles(llvm::cl::Positional,
  llvm::cl::desc("[input object files]"),
  llvm::cl::ZeroOrMore);

// --script is an alias, but cl::alias doesn't work correctly with cl::list.
llvm::cl::list<std::string> ArgLinkerScript("T",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("Linker script"),
  llvm::cl::value_desc("file"));

//===----------------------------------------------------------------------===//
// Namespecs
//===----------------------------------------------------------------------===//
llvm::cl::list<std::string> ArgNameSpecList("l",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("Add the archive or object file specified by namespec to\n"
                 "the list of files to link."),
  llvm::cl::value_desc("namespec"),
  llvm::cl::Prefix);

llvm::cl::alias ArgNameSpecListAlias("library",
  llvm::cl::desc("alias for -l"),
  llvm::cl::aliasopt(ArgNameSpecList));

//===----------------------------------------------------------------------===//
// Attributes
//===----------------------------------------------------------------------===//
llvm::cl::list<bool> ArgWholeArchiveList("whole-archive",
  llvm::cl::ValueDisallowed,
  llvm::cl::desc("For each archive mentioned on the command line after\n"
                 "the --whole-archive option, include all object files\n"
                 "in the archive."));

llvm::cl::list<bool> ArgNoWholeArchiveList("no-whole-archive",
  llvm::cl::ValueDisallowed,
  llvm::cl::desc("Turn off the effect of the --whole-archive option for\n"
                 "subsequent archive files."));

llvm::cl::list<bool> ArgAsNeededList("as-needed",
  llvm::cl::ValueDisallowed,
  llvm::cl::desc("This option affects ELF DT_NEEDED tags for dynamic\n"
                 "libraries mentioned on the command line after the\n"
                 "--as-needed option."));

llvm::cl::list<bool> ArgNoAsNeededList("no-as-needed",
  llvm::cl::ValueDisallowed,
  llvm::cl::desc("Turn off the effect of the --as-needed option for\n"
                 "subsequent dynamic libraries"));

llvm::cl::list<bool> ArgAddNeededList("add-needed",
  llvm::cl::ValueDisallowed,
  llvm::cl::desc("--add-needed causes DT_NEEDED tags are always\n"
                 "emitted for those libraries from DT_NEEDED tags.\n"
                 "This is the default behavior."));

llvm::cl::list<bool> ArgNoAddNeededList("no-add-needed",
  llvm::cl::ValueDisallowed,
  llvm::cl::desc("--no-add-needed causes DT_NEEDED tags will never be\n"
                 "emitted for those libraries from DT_NEEDED tags"));

llvm::cl::list<bool> ArgBDynamicList("Bdynamic",
  llvm::cl::ValueDisallowed,
  llvm::cl::desc("Link against dynamic library"));

llvm::cl::alias ArgBDynamicListAlias1("dy",
  llvm::cl::desc("alias for --Bdynamic"),
  llvm::cl::aliasopt(ArgBDynamicList));

llvm::cl::alias ArgBDynamicListAlias2("call_shared",
  llvm::cl::desc("alias for --Bdynamic"),
  llvm::cl::aliasopt(ArgBDynamicList));

llvm::cl::list<bool> ArgBStaticList("Bstatic",
  llvm::cl::ValueDisallowed,
  llvm::cl::desc("Link against static library"));

llvm::cl::alias ArgBStaticListAlias1("dn",
  llvm::cl::desc("alias for --Bstatic"),
  llvm::cl::aliasopt(ArgBStaticList));

llvm::cl::alias ArgBStaticListAlias2("static",
  llvm::cl::desc("alias for --Bstatic"),
  llvm::cl::aliasopt(ArgBStaticList));

llvm::cl::alias ArgBStaticListAlias3("non_shared",
  llvm::cl::desc("alias for --Bstatic"),
  llvm::cl::aliasopt(ArgBStaticList));

//===----------------------------------------------------------------------===//
// Groups
//===----------------------------------------------------------------------===//
llvm::cl::list<bool> ArgStartGroupList("start-group",
  llvm::cl::ValueDisallowed,
  llvm::cl::desc("start to record a group of archives"));

llvm::cl::alias ArgStartGroupListAlias("(",
  llvm::cl::desc("alias for --start-group"),
  llvm::cl::aliasopt(ArgStartGroupList));

llvm::cl::list<bool> ArgEndGroupList("end-group",
  llvm::cl::ValueDisallowed,
  llvm::cl::desc("stop recording a group of archives"));

llvm::cl::alias ArgEndGroupListAlias(")",
  llvm::cl::desc("alias for --end-group"),
  llvm::cl::aliasopt(ArgEndGroupList));

//===----------------------------------------------------------------------===//
// --defsym
//===----------------------------------------------------------------------===//
llvm::cl::list<std::string> ArgDefSymList("defsym",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("Define a symbol"),
  llvm::cl::value_desc("symbol=expression"));

//===----------------------------------------------------------------------===//
// Help Functions
//===----------------------------------------------------------------------===//
inline bool
CompareAction(const mcld::InputAction* X, const mcld::InputAction* Y)
{
  return (X->position() < Y->position());
}

} // anonymous namespace

using namespace mcld;

//===----------------------------------------------------------------------===//
// PositionalOptions
//===----------------------------------------------------------------------===//
PositionalOptions::PositionalOptions()
  : m_InputObjectFiles(ArgInputObjectFiles),
    m_LinkerScript(ArgLinkerScript),
    m_NameSpecList(ArgNameSpecList),
    m_WholeArchiveList(ArgWholeArchiveList),
    m_NoWholeArchiveList(ArgNoWholeArchiveList),
    m_AsNeededList(ArgAsNeededList),
    m_NoAsNeededList(ArgNoAsNeededList),
    m_AddNeededList(ArgAddNeededList),
    m_NoAddNeededList(ArgNoAddNeededList),
    m_BDynamicList(ArgBDynamicList),
    m_BStaticList(ArgBStaticList),
    m_StartGroupList(ArgStartGroupList),
    m_EndGroupList(ArgEndGroupList),
    m_DefSymList(ArgDefSymList) {
}

size_t PositionalOptions::numOfActions() const
{
  return m_InputObjectFiles.size() +
         m_LinkerScript.size() +
         m_NameSpecList.size() +
         m_WholeArchiveList.size() +
         m_NoWholeArchiveList.size() +
         m_AsNeededList.size() +
         m_NoAsNeededList.size() +
         m_AddNeededList.size() +
         m_NoAddNeededList.size() +
         m_BDynamicList.size() +
         m_BStaticList.size() +
         m_StartGroupList.size() +
         m_EndGroupList.size() +
         m_DefSymList.size();
}

size_t PositionalOptions::numOfInputs() const
{
  return (m_InputObjectFiles.size() +
          m_LinkerScript.size() +
          m_NameSpecList.size());
}

bool PositionalOptions::parse(std::vector<InputAction*>& pActions,
                              const LinkerConfig& pConfig,
                              const LinkerScript& pScript)
{
  if (0 == numOfInputs()) {
    fatal(diag::err_no_inputs);
    return false;
  }

  pActions.reserve(numOfActions());

  // -T/--script
  // FIXME:
  llvm::cl::list<std::string>::iterator sp;
  llvm::cl::list<std::string>::iterator spEnd = m_LinkerScript.end();
  for (sp = m_LinkerScript.begin(); sp != spEnd; ++sp) {
    pActions.push_back(new ScriptAction(0x0,
                                       *sp,
                                       ScriptFile::LDScript,
                                       pScript.directories()));
    pActions.push_back(new ContextAction(0x0));
    pActions.push_back(new MemoryAreaAction(0x0, FileHandle::ReadOnly));
  }

  // --defsym
  llvm::cl::list<std::string>::iterator defsym, dsBegin, dsEnd;
  dsBegin = m_DefSymList.begin();
  dsEnd = m_DefSymList.end();
  for (defsym = dsBegin; defsym != dsEnd; ++defsym) {
    unsigned int pos = m_DefSymList.getPosition(defsym - dsBegin);
    pActions.push_back(new DefSymAction(pos, *defsym));
  }

  // set input
  llvm::cl::list<mcld::sys::fs::Path>::iterator input, inBegin, inEnd;
  inBegin = m_InputObjectFiles.begin();
  inEnd = m_InputObjectFiles.end();
  for (input = inBegin; input != inEnd; ++input) {
    unsigned int pos = m_InputObjectFiles.getPosition(input - inBegin);
    pActions.push_back(new InputFileAction(pos, *input));
    pActions.push_back(new ContextAction(pos));
    pActions.push_back(new MemoryAreaAction(pos, FileHandle::ReadOnly));
  }

  // set -l[namespec]
  llvm::cl::list<std::string>::iterator namespec, nsBegin, nsEnd;
  nsBegin = m_NameSpecList.begin();
  nsEnd = m_NameSpecList.end();
  for (namespec = nsBegin; namespec != nsEnd; ++namespec) {
    unsigned int pos = m_NameSpecList.getPosition(namespec - nsBegin);
    pActions.push_back(new NamespecAction(pos, *namespec,
                                          pScript.directories()));
    pActions.push_back(new ContextAction(pos));
    pActions.push_back(new MemoryAreaAction(pos, FileHandle::ReadOnly));
  }

  // set --whole-archive
  llvm::cl::list<bool>::iterator attr, attrBegin, attrEnd;
  attrBegin = m_WholeArchiveList.begin();
  attrEnd   = m_WholeArchiveList.end();
  for (attr = attrBegin; attr != attrEnd; ++attr) {
    unsigned int pos = m_WholeArchiveList.getPosition(attr - attrBegin);
    pActions.push_back(new WholeArchiveAction(pos));
  }

  // set --no-whole-archive
  attrBegin = m_NoWholeArchiveList.begin();
  attrEnd   = m_NoWholeArchiveList.end();
  for (attr = attrBegin; attr != attrEnd; ++attr) {
    unsigned int pos = m_NoWholeArchiveList.getPosition(attr - attrBegin);
    pActions.push_back(new NoWholeArchiveAction(pos));
  }

  // set --as-needed
  attrBegin = m_AsNeededList.begin();
  attrEnd   = m_AsNeededList.end();
  for (attr = attrBegin; attr != attrEnd; ++attr) {
    unsigned int pos = m_AsNeededList.getPosition(attr - attrBegin);
    pActions.push_back(new AsNeededAction(pos));
  }

  // set --no-as-needed
  attrBegin = m_NoAsNeededList.begin();
  attrEnd   = m_NoAsNeededList.end();
  for (attr = attrBegin; attr != attrEnd; ++attr) {
    unsigned int pos = m_NoAsNeededList.getPosition(attr - attrBegin);
    pActions.push_back(new NoAsNeededAction(pos));
  }

  // set --add--needed
  attrBegin = m_AddNeededList.begin();
  attrEnd   = m_AddNeededList.end();
  for (attr = attrBegin; attr != attrEnd; ++attr) {
    unsigned int pos = m_AddNeededList.getPosition(attr - attrBegin);
    pActions.push_back(new AddNeededAction(pos));
  }

  // set --no-add--needed
  attrBegin = m_NoAddNeededList.begin();
  attrEnd   = m_NoAddNeededList.end();
  for (attr = attrBegin; attr != attrEnd; ++attr) {
    unsigned int pos = m_NoAddNeededList.getPosition(attr - attrBegin);
    pActions.push_back(new NoAddNeededAction(pos));
  }

  // set --Bdynamic
  attrBegin = m_BDynamicList.begin();
  attrEnd   = m_BDynamicList.end();
  for (attr = attrBegin; attr != attrEnd; ++attr) {
    unsigned int pos = m_BDynamicList.getPosition(attr - attrBegin);
    pActions.push_back(new BDynamicAction(pos));
  }

  // set --Bstatic
  attrBegin = m_BStaticList.begin();
  attrEnd   = m_BStaticList.end();
  for (attr = attrBegin; attr != attrEnd; ++attr) {
    unsigned int pos = m_BStaticList.getPosition(attr - attrBegin);
    pActions.push_back(new BStaticAction(pos));
  }

  // set --start-group
  llvm::cl::list<bool>::iterator group, gsBegin, gsEnd;
  gsBegin = m_StartGroupList.begin();
  gsEnd   = m_StartGroupList.end();
  for (group = gsBegin; group != gsEnd; ++group) {
    unsigned int pos = m_StartGroupList.getPosition(group - gsBegin);
    pActions.push_back(new StartGroupAction(pos));
  }

  // set --end-group
  gsBegin = m_EndGroupList.begin();
  gsEnd   = m_EndGroupList.end();
  for (group = gsBegin; group != gsEnd; ++group) {
    unsigned int pos = m_EndGroupList.getPosition(group - gsBegin);
    pActions.push_back(new EndGroupAction(pos));
  }

  // stable sort
  std::stable_sort(pActions.begin(), pActions.end(), CompareAction);

  return true;
}

