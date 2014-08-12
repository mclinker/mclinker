add_mcld_library(MCLDMipsLDBackend
  MipsDiagnostic.cpp
  MipsELFDynamic.cpp
  MipsEmulation.cpp
  MipsGNUInfo.cpp
  MipsGOT.cpp
  MipsGOTPLT.cpp
  MipsLA25Stub.cpp
  MipsLDBackend.cpp
  MipsPLT.cpp
  MipsRelocator.cpp
  )

add_subdirectory(TargetInfo)

target_link_libraries(MCLDMipsLDBackend
  MCLDMipsTargetInfo
  MCLDCore
  MCLDFragment
  MCLDObject
  MCLDTarget
  )
