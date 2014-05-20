//===- ScriptOptions.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/ScriptOptions.h>
#include <mcld/LinkerScript.h>
#include <mcld/ADT/StringEntry.h>
#include <mcld/Support/MsgHandling.h>

namespace {

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

} // anonymous namespace

using namespace mcld;

//===----------------------------------------------------------------------===//
// ScriptOptions
//===----------------------------------------------------------------------===//
ScriptOptions::ScriptOptions()
  : m_WrapList(ArgWrapList),
    m_PortList(ArgPortList),
    m_AddressMapList(ArgAddressMapList),
    m_BssSegAddr(ArgBssSegAddr),
    m_DataSegAddr(ArgDataSegAddr),
    m_TextSegAddr(ArgTextSegAddr) {
}

bool ScriptOptions::parse(LinkerScript& pScript)
{
  // set up rename map, for --wrap
  llvm::cl::list<std::string>::iterator wname;
  llvm::cl::list<std::string>::iterator wnameEnd = ArgWrapList.end();
  for (wname = ArgWrapList.begin(); wname != wnameEnd; ++wname) {
    bool exist = false;

    // add wname -> __wrap_wname
    StringEntry<llvm::StringRef>* to_wrap =
                                     pScript.renameMap().insert(*wname, exist);

    std::string to_wrap_str = "__wrap_" + *wname;
    to_wrap->setValue(to_wrap_str);

    if (exist)
      warning(mcld::diag::rewrap) << *wname << to_wrap_str;

    // add __real_wname -> wname
    std::string from_real_str = "__real_" + *wname;
    StringEntry<llvm::StringRef>* from_real =
                              pScript.renameMap().insert(from_real_str, exist);
    from_real->setValue(*wname);
    if (exist)
      mcld::warning(mcld::diag::rewrap) << *wname << from_real_str;
  }

  // set up rename map, for --portable
  llvm::cl::list<std::string>::iterator pname;
  llvm::cl::list<std::string>::iterator pnameEnd = ArgPortList.end();
  for (pname = ArgPortList.begin(); pname != pnameEnd; ++pname) {
    bool exist = false;

    // add pname -> pname_portable
    StringEntry<llvm::StringRef>* to_port =
                                     pScript.renameMap().insert(*pname, exist);

    std::string to_port_str = *pname + "_portable";
    to_port->setValue(to_port_str);

    if (exist)
      warning(mcld::diag::rewrap) << *pname << to_port_str;

    // add __real_pname -> pname
    std::string from_real_str = "__real_" + *pname;
    StringEntry<llvm::StringRef>* from_real =
                              pScript.renameMap().insert(from_real_str, exist);

    from_real->setValue(*pname);
    if (exist)
      warning(mcld::diag::rewrap) << *pname << from_real_str;
  } // end of for

  // set --section-start SECTION=ADDRESS
  for (llvm::cl::list<std::string>::iterator
       it = ArgAddressMapList.begin(), ie = ArgAddressMapList.end();
       it != ie; ++it) {
    // FIXME: Add a cl::parser
    size_t pos = (*it).find_last_of('=');
    llvm::StringRef script(*it);
    uint64_t address = 0x0;
    script.substr(pos + 1).getAsInteger(0, address);
    bool exist = false;
    StringEntry<uint64_t>* addr_mapping =
                     pScript.addressMap().insert(script.substr(0, pos), exist);
    addr_mapping->setValue(address);
  }

  // set -Tbss [address]
  if (-1U != ArgBssSegAddr) {
    bool exist = false;
    StringEntry<uint64_t>* bss_mapping =
                                    pScript.addressMap().insert(".bss", exist);
    bss_mapping->setValue(ArgBssSegAddr);
  }

  // set -Tdata [address]
  if (-1U != ArgDataSegAddr) {
    bool exist = false;
    StringEntry<uint64_t>* data_mapping =
                                   pScript.addressMap().insert(".data", exist);
    data_mapping->setValue(ArgDataSegAddr);
  }

  // set -Ttext [address]
  if (-1U != ArgTextSegAddr) {
    bool exist = false;
    StringEntry<uint64_t>* text_mapping =
                                   pScript.addressMap().insert(".text", exist);
    text_mapping->setValue(ArgTextSegAddr);
  }

  return true;
}

