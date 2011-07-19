/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Support/CommandLine.h>
#include <llvm/ADT/StringRef.h>

#ifdef MCLD_DEBUG
#include <iostream>
using namespace std;
#endif

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
  cerr << "==================" << endl;
  cerr << "ArgName=" << ArgName.str() << endl;
  cerr << "Arg=" << Arg.str() << endl;
  cerr << "------------------" << endl;
  Val.assign<llvm::StringRef::const_iterator>(Arg.begin(), Arg.end());
  return false;
}

void parser<mcld::sys::fs::Path>::printOptionDiff(const llvm::cl::Option &O,
                                                  const mcld::sys::fs::Path &V,
                                                  parser<mcld::sys::fs::Path>::OptVal Default,
                                                  size_t GlobalWidth) const
{
}

void parser<mcld::sys::fs::Path>::anchor()
{
}

//--------------------------------------------------
// parser<mcld::MCLDDirectory>
//
bool parser<mcld::MCLDDirectory>::parse(llvm::cl::Option &O,
                                llvm::StringRef ArgName,
                                llvm::StringRef Arg,
                                mcld::MCLDDirectory &Val)
{
	cerr << "wa wa" << endl;
}

void parser<mcld::MCLDDirectory>::printOptionDiff(const llvm::cl::Option &O,
                                                  const mcld::MCLDDirectory &V,
                                                  parser<mcld::MCLDDirectory>::OptVal Default,
                                                  size_t GlobalWidth) const
{
}

void parser<mcld::MCLDDirectory>::anchor()
{
}

