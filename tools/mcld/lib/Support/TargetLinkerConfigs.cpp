//===- TargetLinkerConfigs.cpp --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "alone/Config/Config.h"
#include "alone/Support/TargetLinkerConfigs.h"

using namespace alone;

#ifdef TARGET_BUILD
static const char* gDefaultDyld = "/system/bin/linker";
static const char* gDefaultSysroot = "/system";
#else
static const char* gDefaultDyld = "/usr/lib/ld.so.1";
static const char* gDefaultSysroot = "/";
#endif

//===----------------------------------------------------------------------===//
// ARM
//===----------------------------------------------------------------------===//
#if defined(PROVIDE_ARM_CODEGEN)
ARMLinkerConfig::ARMLinkerConfig() : LinkerConfig(DEFAULT_ARM_TRIPLE_STRING) {

  // set up target-dependent constraints of attributes
  getLDInfo()->attrFactory().constraint().enableWholeArchive();
  getLDInfo()->attrFactory().constraint().disableAsNeeded();
  getLDInfo()->attrFactory().constraint().setSharedSystem();

  // set up the predefined attributes
  getLDInfo()->attrFactory().predefined().setWholeArchive();
  getLDInfo()->attrFactory().predefined().setDynamic();

  // set up target dependent options
  if (getLDInfo()->options().sysroot().empty()) {
    getLDInfo()->options().setSysroot(gDefaultSysroot);
  }

  if (!getLDInfo()->options().hasDyld()) {
    getLDInfo()->options().setDyld(gDefaultDyld);
  }
}
#endif // defined(PROVIDE_ARM_CODEGEN)

//===----------------------------------------------------------------------===//
// Mips
//===----------------------------------------------------------------------===//
#if defined(PROVIDE_MIPS_CODEGEN)
MipsLinkerConfig::MipsLinkerConfig()
  : LinkerConfig(DEFAULT_MIPS_TRIPLE_STRING) {

  // set up target-dependent constraints of attibutes
  getLDInfo()->attrFactory().constraint().enableWholeArchive();
  getLDInfo()->attrFactory().constraint().disableAsNeeded();
  getLDInfo()->attrFactory().constraint().setSharedSystem();

  // set up the predefined attributes
  getLDInfo()->attrFactory().predefined().setWholeArchive();
  getLDInfo()->attrFactory().predefined().setDynamic();

  // set up target dependent options
  if (getLDInfo()->options().sysroot().empty()) {
    getLDInfo()->options().setSysroot(gDefaultSysroot);
  }

  if (!getLDInfo()->options().hasDyld()) {
    getLDInfo()->options().setDyld(gDefaultDyld);
  }
}
#endif // defined(PROVIDE_MIPS_CODEGEN)

//===----------------------------------------------------------------------===//
// X86 and X86_64
//===----------------------------------------------------------------------===//
#if defined(PROVIDE_X86_CODEGEN)
X86FamilyLinkerConfigBase::X86FamilyLinkerConfigBase(const std::string& pTriple)
  : LinkerConfig(pTriple) {
  // set up target-dependent constraints of attibutes
  getLDInfo()->attrFactory().constraint().enableWholeArchive();
  getLDInfo()->attrFactory().constraint().disableAsNeeded();
  getLDInfo()->attrFactory().constraint().setSharedSystem();

  // set up the predefined attributes
  getLDInfo()->attrFactory().predefined().setWholeArchive();
  getLDInfo()->attrFactory().predefined().setDynamic();

  // set up target dependent options
  if (getLDInfo()->options().sysroot().empty()) {
    getLDInfo()->options().setSysroot(gDefaultSysroot);
  }

  if (!getLDInfo()->options().hasDyld()) {
    getLDInfo()->options().setDyld(gDefaultDyld);
  }
}

X86_32LinkerConfig::X86_32LinkerConfig()
  : X86FamilyLinkerConfigBase(DEFAULT_X86_TRIPLE_STRING) {
}

X86_64LinkerConfig::X86_64LinkerConfig()
  : X86FamilyLinkerConfigBase(DEFAULT_X86_64_TRIPLE_STRING) {
}
#endif // defined(PROVIDE_X86_CODEGEN)
