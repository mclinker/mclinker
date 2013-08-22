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

//===----------------------------------------------------------------------===//
// Positional Options
// The meaning of a positional option depends on its position and its related
// positions with the other positional options. There are four kinds of
// positional options:
//   1. Inputs, object files, such as /tmp/XXXX.o
//   2. Namespecs, short names of libraries. A namespec may refer to an archive
//      or a shared library. For example, -lm.
//   3. Attributes of inputs. Attributes describe inputs appears after them.
//      For example, --as-needed and --whole-archive.
//   4. Groups. A Group is a set of archives. Linkers repeatedly read archives
//      in groups until there is no new undefined symbols.
//   5. Definitions of symbols. --defsym option depends on
//===----------------------------------------------------------------------===//
// Inputs
//===----------------------------------------------------------------------===//
static llvm::cl::list<mcld::sys::fs::Path>
ArgInputObjectFiles(llvm::cl::Positional,
                    llvm::cl::desc("[input object files]"),
                    llvm::cl::ZeroOrMore);

//===----------------------------------------------------------------------===//
// Namespecs
//===----------------------------------------------------------------------===//
static llvm::cl::list<std::string>
ArgNameSpecList("l",
       llvm::cl::ZeroOrMore,
       llvm::cl::desc("Add the archive or object file specified by namespec to "
                      "the list of files to link."),
       llvm::cl::value_desc("namespec"),
       llvm::cl::Prefix);

static llvm::cl::alias
ArgNameSpecListAlias("library",
                     llvm::cl::desc("alias for -l"),
                     llvm::cl::aliasopt(ArgNameSpecList));

//===----------------------------------------------------------------------===//
// Attributes
//===----------------------------------------------------------------------===//
static llvm::cl::list<bool>
ArgWholeArchiveList("whole-archive",
          llvm::cl::ValueDisallowed,
          llvm::cl::desc("For each archive mentioned on the command line after "
                         "the --whole-archive option, include all object files "
                         "in the archive."));

static llvm::cl::list<bool>
ArgNoWholeArchiveList("no-whole-archive",
         llvm::cl::ValueDisallowed,
         llvm::cl::desc("Turn off the effect of the --whole-archive option for "
                        "subsequent archive files."));

static llvm::cl::list<bool>
ArgAsNeededList("as-needed",
            llvm::cl::ValueDisallowed,
            llvm::cl::desc("This option affects ELF DT_NEEDED tags for dynamic "
                           "libraries mentioned on the command line after the "
                           "--as-needed option."));

static llvm::cl::list<bool>
ArgNoAsNeededList("no-as-needed",
             llvm::cl::ValueDisallowed,
             llvm::cl::desc("Turn off the effect of the --as-needed option for "
                            "subsequent dynamic libraries"));

static llvm::cl::list<bool>
ArgAddNeededList("add-needed",
                 llvm::cl::ValueDisallowed,
                 llvm::cl::desc("--add-needed causes DT_NEEDED tags are always "
                             "emitted for those libraries from DT_NEEDED tags. "
                             "This is the default behavior."));

static llvm::cl::list<bool>
ArgNoAddNeededList("no-add-needed",
            llvm::cl::ValueDisallowed,
            llvm::cl::desc("--no-add-needed causes DT_NEEDED tags will never be "
                           "emitted for those libraries from DT_NEEDED tags"));

static llvm::cl::list<bool>
ArgBDynamicList("Bdynamic",
                llvm::cl::ValueDisallowed,
                llvm::cl::desc("Link against dynamic library"));

static llvm::cl::alias
ArgBDynamicListAlias1("dy",
                      llvm::cl::desc("alias for --Bdynamic"),
                      llvm::cl::aliasopt(ArgBDynamicList));

static llvm::cl::alias
ArgBDynamicListAlias2("call_shared",
                      llvm::cl::desc("alias for --Bdynamic"),
                      llvm::cl::aliasopt(ArgBDynamicList));

static llvm::cl::list<bool>
ArgBStaticList("Bstatic",
               llvm::cl::ValueDisallowed,
               llvm::cl::desc("Link against static library"));

static llvm::cl::alias
ArgBStaticListAlias1("dn",
                     llvm::cl::desc("alias for --Bstatic"),
                     llvm::cl::aliasopt(ArgBStaticList));

static llvm::cl::alias
ArgBStaticListAlias2("static",
                     llvm::cl::desc("alias for --Bstatic"),
                     llvm::cl::aliasopt(ArgBStaticList));

static llvm::cl::alias
ArgBStaticListAlias3("non_shared",
                     llvm::cl::desc("alias for --Bstatic"),
                     llvm::cl::aliasopt(ArgBStaticList));

//===----------------------------------------------------------------------===//
// Groups
//===----------------------------------------------------------------------===//
static llvm::cl::list<bool>
ArgStartGroupList("start-group",
                  llvm::cl::ValueDisallowed,
                  llvm::cl::desc("start to record a group of archives"));

static llvm::cl::alias
ArgStartGroupListAlias("(",
                       llvm::cl::desc("alias for --start-group"),
                       llvm::cl::aliasopt(ArgStartGroupList));

static llvm::cl::list<bool>
ArgEndGroupList("end-group",
                llvm::cl::ValueDisallowed,
                llvm::cl::desc("stop recording a group of archives"));

static llvm::cl::alias
ArgEndGroupListAlias(")",
                     llvm::cl::desc("alias for --end-group"),
                     llvm::cl::aliasopt(ArgEndGroupList));

//===----------------------------------------------------------------------===//
// --defsym
//===----------------------------------------------------------------------===//
static llvm::cl::list<std::string>
ArgDefSymList("defsym",
              llvm::cl::ZeroOrMore,
              llvm::cl::desc("Define a symbol"),
              llvm::cl::value_desc("symbol=expression"));

#endif

