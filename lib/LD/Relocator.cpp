//===- Relocator.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Config/Config.h>
#include <mcld/Fragment/Fragment.h>
#include <mcld/LD/LDContext.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/Relocator.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/SectionData.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Module.h>
#ifdef HAVE_CXXABI_H
#include <cxxabi.h>
#endif
#include <sstream>

using namespace mcld;

//===----------------------------------------------------------------------===//
// Helper functions
//===----------------------------------------------------------------------===//
std::string demangleSymbol(const std::string& mangled_name) {
#ifdef HAVE_CXXABI_H
  // __cxa_demangle needs manually handle the memory release, so we wrap
  // it into this helper function.
  size_t output_leng;
  int status;
  char* buffer = abi::__cxa_demangle(mangled_name.c_str(), /*buffer=*/0,
                                     &output_leng, &status);
  if (status != 0) { // Failed
    return mangled_name;
  }
  std::string demangled_name(buffer);
  free(buffer);

  return demangled_name;
#else
  return mangled_name;
#endif
}


//===----------------------------------------------------------------------===//
// Relocator
//===----------------------------------------------------------------------===//
Relocator::~Relocator()
{
}

void Relocator::partialScanRelocation(Relocation& pReloc,
                                      Module& pModule,
                                      const LDSection& pSection)
{
  // if we meet a section symbol
  if (pReloc.symInfo()->type() == ResolveInfo::Section) {
    LDSymbol* input_sym = pReloc.symInfo()->outSymbol();

    // 1. update the relocation target offset
    assert(input_sym->hasFragRef());
    uint64_t offset = input_sym->fragRef()->getOutputOffset();
    pReloc.target() += offset;

    // 2. get output section symbol
    // get the output LDSection which the symbol defined in
    const LDSection& out_sect =
                        input_sym->fragRef()->frag()->getParent()->getSection();
    ResolveInfo* sym_info =
                     pModule.getSectionSymbolSet().get(out_sect)->resolveInfo();
    // set relocation target symbol to the output section symbol's resolveInfo
    pReloc.setSymInfo(sym_info);
  }
}

void Relocator::issueUndefRef(Relocation& pReloc,
                              LDSection& pSection,
                              Input& pInput)
{
  FragmentRef::Offset undef_sym_pos = pReloc.targetRef().offset();
  std::string sect_name(pSection.name());
  sect_name = sect_name.substr(sect_name.find('.', /*pos=*/1));  // Drop .rel(a) prefix

  std::string reloc_sym(pReloc.symInfo()->name());
  if (reloc_sym.substr(0, 2) == "_Z")
    reloc_sym = demangleSymbol(reloc_sym);

  std::stringstream ss;
  ss << "0x" << std::hex << undef_sym_pos;
  std::string undef_sym_pos_hex(ss.str());

  if (sect_name.substr(0, 5) != ".text") {
    // Function name is only valid for text section
    fatal(diag::undefined_reference) << reloc_sym
                                     << pInput.path()
                                     << sect_name
                                     << undef_sym_pos_hex;
    return;
  }

  std::string caller_file_name;
  std::string caller_func_name;
  for (LDContext::sym_iterator i = pInput.context()->symTabBegin(),
       e = pInput.context()->symTabEnd(); i != e; ++i) {
    LDSymbol& sym = **i;
    if (sym.resolveInfo()->type() == ResolveInfo::File)
      caller_file_name = sym.resolveInfo()->name();

    if (sym.resolveInfo()->type() == ResolveInfo::Function &&
        sym.value() <= undef_sym_pos &&
        sym.value() + sym.size() > undef_sym_pos) {
      caller_func_name = sym.name();
      break;
    }
  }

  if (caller_func_name.substr(0, 2) == "_Z")
    caller_func_name = demangleSymbol(caller_func_name);

  fatal(diag::undefined_reference_text) << reloc_sym
                                        << pInput.path()
                                        << caller_file_name
                                        << caller_func_name;
}
