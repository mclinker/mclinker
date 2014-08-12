add_mcld_library(MCLDAArch64LDBackend
  AArch64Diagnostic.cpp
  AArch64ELFDynamic.cpp
  AArch64Emulation.cpp
  AArch64GOT.cpp
  AArch64LDBackend.cpp
  AArch64PLT.cpp
  AArch64Relocator.cpp
  )

add_subdirectory(TargetInfo)

target_link_libraries(MCLDAArch64LDBackend
  MCLDAArch64TargetInfo
  MCLDCore
  MCLDFragment
  MCLDObject
  MCLDTarget
  )
