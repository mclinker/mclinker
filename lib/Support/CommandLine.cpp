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

using namespace llvm;
using namespace llvm::cl;

//--------------------------------------------------
// parser<mcld::sys::fs::Path>
//
bool parser<mcld::sys::fs::Path>::parse(llvm::cl::Option &O,
                       llvm::StringRef ArgName,
                       llvm::StringRef Arg,
                       mcld::sys::fs::Path &Val)
{
  Val.assign<llvm::StringRef::const_iterator>(Arg.begin(), Arg.end());
  return false;
}

static const size_t MaxOptWidth = 8;  // arbitrary spacing for printOptionDiff

void parser<mcld::sys::fs::Path>::printOptionDiff(const llvm::cl::Option &O,
                                                  const mcld::sys::fs::Path &V,
                                                  parser<mcld::sys::fs::Path>::OptVal Default,
                                                  size_t GlobalWidth) const
{
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

void parser<mcld::sys::fs::Path>::anchor()
{
  // do nothing
}

//--------------------------------------------------
// parser<mcld::MCLDDirectory>
//
bool parser<mcld::MCLDDirectory>::parse(llvm::cl::Option &O,
                                llvm::StringRef ArgName,
                                llvm::StringRef Arg,
                                mcld::MCLDDirectory &Val)
{
  Val.assign(Arg);
  return false;
}

void parser<mcld::MCLDDirectory>::printOptionDiff(const llvm::cl::Option &O,
                                                  const mcld::MCLDDirectory &V,
                                                  parser<mcld::MCLDDirectory>::OptVal Default,
                                                  size_t GlobalWidth) const
{
  printOptionName(O, GlobalWidth);
  outs() << "= " << V.name();
  size_t VSize = V.name().size();
  size_t NumSpaces = MaxOptWidth > VSize ? MaxOptWidth - VSize : 0;
  outs().indent(NumSpaces) << " (default: ";
  if (Default.hasValue())
    outs() << Default.getValue().name();
  else
    outs() << "*no default*";
  outs() << ")\n";
}

void parser<mcld::MCLDDirectory>::anchor()
{
  // do nothing
}

