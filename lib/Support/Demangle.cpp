//===- Demangle.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/Demangle.h>

#ifdef HAVE_CXXABI_H
#include <cxxabi.h>
#endif

namespace mcld {

std::string demangleName(const std::string& mangled_name) {
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

} // namespace mcld
