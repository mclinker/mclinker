//===- CommandLine.h ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SUPPORT_COMMANDLINE_H_
#define MCLD_SUPPORT_COMMANDLINE_H_
#include "mcld/MC/ZOption.h"
#include "mcld/Support/FileSystem.h"

#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Triple.h>
#include <llvm/Support/CommandLine.h>

#include <string>

namespace llvm {
namespace cl {

//===----------------------------------------------------------------------===//
// SearchDirParser
//===----------------------------------------------------------------------===//
class SearchDirParser : public basic_parser<std::string> {
 public:
  SearchDirParser(Option &O) : basic_parser(O) { }

  // parse - Return true on error.
  bool parse(Option& pOption,
             StringRef pArgName,
             StringRef pArg,
             std::string& pValue);

  const char* getValueName() const { return "searchdir"; }

  void printOptionDiff(const Option& pOption,
                       StringRef pValue,
                       OptVal pDefault,
                       size_t pGlobalWidth) const;

  void anchor();
};

//===----------------------------------------------------------------------===//
// FalseParser
//===----------------------------------------------------------------------===//
class FalseParser : public parser<bool> {
 public:
  FalseParser(Option &O) : parser<bool>(O) { }

  // parse - Return true on error.
  bool parse(cl::Option& O, StringRef ArgName, StringRef Arg, bool& Val) {
    if (cl::parser<bool>::parse(O, ArgName, Arg, Val))
      return false;
    Val = false;
    return false;
  }
};

//===----------------------------------------------------------------------===//
// parser<mcld::sys::fs::Path>
//===----------------------------------------------------------------------===//
template <>
class parser<mcld::sys::fs::Path> : public basic_parser<mcld::sys::fs::Path> {
 public:
  parser(Option &O) : basic_parser(O) { }

  bool parse(Option& O,
             StringRef ArgName,
             StringRef Arg,
             mcld::sys::fs::Path& Val);

  virtual const char* getValueName() const { return "path"; }
  void printOptionDiff(const Option& O,
                       const mcld::sys::fs::Path& V,
                       OptVal Default,
                       size_t GlobalWidth) const;
  virtual void anchor();
};

//===----------------------------------------------------------------------===//
// parser<mcld::ZOption>
//===----------------------------------------------------------------------===//
template <>
class parser<mcld::ZOption> : public basic_parser<mcld::ZOption> {
 public:
  parser(Option &O) : basic_parser(O) { }

  bool parse(Option& O, StringRef ArgName, StringRef Arg, mcld::ZOption& Val);

  virtual const char* getValueName() const { return "z-option"; }
  void printOptionDiff(const Option& O,
                       const mcld::ZOption& V,
                       OptVal Default,
                       size_t GlobalWidth) const;
  virtual void anchor();
};

}  // namespace cl
}  // namespace llvm

#endif  // MCLD_SUPPORT_COMMANDLINE_H_
