//===- ScriptOptions.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LDLITE_SCRIPT_OPTIONS_H
#define MCLD_LDLITE_SCRIPT_OPTIONS_H

//===----------------------------------------------------------------------===//
// Script Options
// Script options are used to modify the default link script. Some positional
// options, such as --defsym, also can modify default link script is not listed
// here. These special options belong to Positional Options.
//===----------------------------------------------------------------------===//
static llvm::cl::list<std::string>
ArgWrapList("wrap",
            llvm::cl::ZeroOrMore,
            llvm::cl::desc("Use a wrap function fo symbol."),
            llvm::cl::value_desc("symbol"));

static llvm::cl::list<std::string>
ArgPortList("portable",
            llvm::cl::ZeroOrMore,
            llvm::cl::desc("Use a portable function fo symbol."),
            llvm::cl::value_desc("symbol"));

static llvm::cl::list<std::string>
ArgAddressMapList("section-start",
                  llvm::cl::ZeroOrMore,
                  llvm::cl::desc("Locate a output section at the given absolute address"),
                  llvm::cl::value_desc("Set address of section"),
                  llvm::cl::Prefix);

static llvm::cl::opt<unsigned long long>
ArgBssSegAddr("Tbss",
              llvm::cl::desc("Set the address of the bss segment"),
              llvm::cl::init(-1U));

static llvm::cl::opt<unsigned long long>
ArgDataSegAddr("Tdata",
               llvm::cl::desc("Set the address of the data segment"),
               llvm::cl::init(-1U));

static llvm::cl::opt<unsigned long long>
ArgTextSegAddr("Ttext",
               llvm::cl::desc("Set the address of the text segment"),
               llvm::cl::init(-1U));

static llvm::cl::alias
ArgTextSegAddrAlias("Ttext-segment",
                    llvm::cl::desc("alias for -Ttext"),
                    llvm::cl::aliasopt(ArgTextSegAddr));


#endif

