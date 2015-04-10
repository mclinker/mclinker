add_llvm_library(MCLDAArch64LDBackend
  AArch64Diagnostic.cpp
  AArch64ELFDynamic.cpp
  AArch64Emulation.cpp
  AArch64GOT.cpp
  AArch64LDBackend.cpp
  AArch64PLT.cpp
  AArch64Relocator.cpp
  LINK_LIBS
    MCLDAArch64TargetInfo
    MCLDCore
    MCLDFragment
    MCLDObject
    MCLDTarget
  )

add_subdirectory(TargetInfo)
