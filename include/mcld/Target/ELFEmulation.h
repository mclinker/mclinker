//===- ELFEmulation.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TARGET_ELFEMULATION_H
#define MCLD_TARGET_ELFEMULATION_H

namespace mcld {

class LinkerConfig;
class LinkerScript;

bool MCLDEmulateELF(LinkerScript& pScript, LinkerConfig& pConfig);

} // namespace of mcld

#endif

