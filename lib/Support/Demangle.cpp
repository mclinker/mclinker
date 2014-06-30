//===- Demangle.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Config/Config.h>
#include <mcld/Support/Demangle.h>

#ifdef HAVE_CXXABI_H
#include <cxxabi.h>
#endif

namespace mcld {

std::string demangleName(const std::string& pName) {
#ifdef HAVE_CXXABI_H
  // Spoil names of symbols with C linkage, so use an heuristic approach to
  // check if the name should be demangled.
  if (pName.substr(0, 2) != "_Z")
    return pName;
  // __cxa_demangle needs manually handle the memory release, so we wrap
  // it into this helper function.
  size_t output_leng;
  int status;
  char* buffer = abi::__cxa_demangle(pName.c_str(), /*buffer=*/0,
                                     &output_leng, &status);
  if (status != 0) { // Failed
    return pName;
  }
  std::string result(buffer);
  free(buffer);

  return result;
#else
  return pName;
#endif
}

} // namespace mcld
