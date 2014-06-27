//===- Demangle.h ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SUPPORT_DEMANGLE_H
#define MCLD_SUPPORT_DEMANGLE_H

#include <string>

namespace mcld {

std::string demangleName(const std::string& mangled_name);

} // namespace mcld

#endif
