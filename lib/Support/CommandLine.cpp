//===- CommandLine.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Support/CommandLine.h"

#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/StringSwitch.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/ErrorHandling.h>

static const size_t MaxOptWidth = 8;  // arbitrary spacing for printOptionDiff

namespace llvm {
namespace cl {

//===----------------------------------------------------------------------===//
// SearchDirParser
//===----------------------------------------------------------------------===//
// parse - Return true on error.
bool SearchDirParser::parse(Option& pOption,
                            StringRef pArgName,
                            StringRef pArg,
                            std::string& pValue) {
  char separator = *(pArgName.data() + 1);
  if (separator == '=')
    pValue = '=';
  pValue += pArg.str();
  return false;
}

void SearchDirParser::printOptionDiff(const Option& pOption,
                                      StringRef pValue,
                                      OptVal pDefault,
                                      size_t pGlobalWidth) const {
  printOptionName(pOption, pGlobalWidth);
  outs() << "= " << pValue;
  size_t NumSpaces =
      MaxOptWidth > pValue.size() ? MaxOptWidth - pValue.size() : 0;
  outs().indent(NumSpaces) << " (default: ";
  if (pDefault.hasValue())
    outs() << pDefault.getValue();
  else
    outs() << "*no default*";
  outs() << ")\n";
}

void SearchDirParser::anchor() {
  // do nothing
}

//===----------------------------------------------------------------------===//
// parser<mcld::sys::fs::Path>
//===----------------------------------------------------------------------===//
bool parser<mcld::sys::fs::Path>::parse(llvm::cl::Option& O,
                                        llvm::StringRef ArgName,
                                        llvm::StringRef Arg,
                                        mcld::sys::fs::Path& Val) {
  Val.assign<llvm::StringRef::const_iterator>(Arg.begin(), Arg.end());
  return false;
}

void parser<mcld::sys::fs::Path>::printOptionDiff(
    const llvm::cl::Option& O,
    const mcld::sys::fs::Path& V,
    parser<mcld::sys::fs::Path>::OptVal Default,
    size_t GlobalWidth) const {
  printOptionName(O, GlobalWidth);
  outs() << "= " << V;
  size_t VSize = V.native().size();
  size_t NumSpaces = MaxOptWidth > VSize ? MaxOptWidth - VSize : 0;
  outs().indent(NumSpaces) << " (default: ";
  if (Default.hasValue())
    outs() << Default.getValue().c_str();
  else
    outs() << "*no default*";
  outs() << ")\n";
}

void parser<mcld::sys::fs::Path>::anchor() {
  // do nothing
}

//===----------------------------------------------------------------------===//
// parser<mcld::ZOption>
//===----------------------------------------------------------------------===//
bool parser<mcld::ZOption>::parse(llvm::cl::Option& O,
                                  llvm::StringRef ArgName,
                                  llvm::StringRef Arg,
                                  mcld::ZOption& Val) {
  if (Arg.equals("combreloc"))
    Val.setKind(mcld::ZOption::CombReloc);
  else if (Arg.equals("nocombreloc"))
    Val.setKind(mcld::ZOption::NoCombReloc);
  else if (Arg.equals("defs"))
    Val.setKind(mcld::ZOption::Defs);
  else if (Arg.equals("execstack"))
    Val.setKind(mcld::ZOption::ExecStack);
  else if (Arg.equals("noexecstack"))
    Val.setKind(mcld::ZOption::NoExecStack);
  else if (Arg.equals("initfirst"))
    Val.setKind(mcld::ZOption::InitFirst);
  else if (Arg.equals("interpose"))
    Val.setKind(mcld::ZOption::InterPose);
  else if (Arg.equals("loadfltr"))
    Val.setKind(mcld::ZOption::LoadFltr);
  else if (Arg.equals("muldefs"))
    Val.setKind(mcld::ZOption::MulDefs);
  else if (Arg.equals("nocopyreloc"))
    Val.setKind(mcld::ZOption::NoCopyReloc);
  else if (Arg.equals("nodefaultlib"))
    Val.setKind(mcld::ZOption::NoDefaultLib);
  else if (Arg.equals("nodelete"))
    Val.setKind(mcld::ZOption::NoDelete);
  else if (Arg.equals("nodlopen"))
    Val.setKind(mcld::ZOption::NoDLOpen);
  else if (Arg.equals("nodump"))
    Val.setKind(mcld::ZOption::NoDump);
  else if (Arg.equals("relro"))
    Val.setKind(mcld::ZOption::Relro);
  else if (Arg.equals("norelro"))
    Val.setKind(mcld::ZOption::NoRelro);
  else if (Arg.equals("lazy"))
    Val.setKind(mcld::ZOption::Lazy);
  else if (Arg.equals("now"))
    Val.setKind(mcld::ZOption::Now);
  else if (Arg.equals("origin"))
    Val.setKind(mcld::ZOption::Origin);
  else if (Arg.startswith("common-page-size=")) {
    Val.setKind(mcld::ZOption::CommPageSize);
    long long unsigned size = 0;
    Arg.drop_front(17).getAsInteger(0, size);
    Val.setPageSize(static_cast<uint64_t>(size));
  } else if (Arg.startswith("max-page-size=")) {
    Val.setKind(mcld::ZOption::MaxPageSize);
    long long unsigned size = 0;
    Arg.drop_front(14).getAsInteger(0, size);
    Val.setPageSize(static_cast<uint64_t>(size));
  }

  if (mcld::ZOption::Unknown == Val.kind())
    llvm::report_fatal_error(llvm::Twine("unknown -z option: `") + Arg +
                             llvm::Twine("'\n"));
  return false;
}

void parser<mcld::ZOption>::printOptionDiff(
    const llvm::cl::Option& O,
    const mcld::ZOption& V,
    parser<mcld::ZOption>::OptVal Default,
    size_t GlobalWidth) const {
  // TODO
}

void parser<mcld::ZOption>::anchor() {
  // do nothing
}

}  // namespace cl
}  // namespace llvm
