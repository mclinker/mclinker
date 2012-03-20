//===- CommandLine.h ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_COMMANDLINE_H
#define MCLD_COMMANDLINE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/CommandLine.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/MC/MCLDDirectory.h>
#include <mcld/MC/ZOption.h>

//--------------------------------------------------
// parser<mcld::sys::fs::Path>
//
namespace llvm {
namespace cl {

template<>
class parser<mcld::sys::fs::Path> : public basic_parser<mcld::sys::fs::Path>
{
public:
  bool parse(Option &O,
             StringRef ArgName,
             StringRef Arg,
             mcld::sys::fs::Path &Val);

  virtual const char *getValueName() const { return "path"; }
  void printOptionDiff(const Option &O,
                       const mcld::sys::fs::Path &V,
                       OptVal Default,
                       size_t GlobalWidth) const;
  virtual void anchor();
};

//--------------------------------------------------
// parser<mcld::MCLDDirectory>
//
template<>
class parser<mcld::MCLDDirectory> : public llvm::cl::basic_parser<mcld::MCLDDirectory>
{
public:
  bool parse(Option &O, StringRef ArgName, StringRef Arg, mcld::MCLDDirectory &Val);

  virtual const char *getValueName() const { return "directory"; }
  void printOptionDiff(const Option &O,
                       const mcld::MCLDDirectory &V,
                       OptVal Default,
                       size_t GlobalWidth) const;
  virtual void anchor();
};

//--------------------------------------------------
// parser<mcld::ZOption>
//
template<>
class parser<mcld::ZOption> : public llvm::cl::basic_parser<mcld::ZOption>
{
public:
  bool parse(Option &O, StringRef ArgName, StringRef Arg, mcld::ZOption &Val);

  virtual const char *getValueName() const { return "z-option"; }
  void printOptionDiff(const Option &O,
                       const mcld::ZOption &V,
                       OptVal Default,
                       size_t GlobalWidth) const;
  virtual void anchor();
};

} // namespace of cl
} // namespace of llvm

#endif

