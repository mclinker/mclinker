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

target_link_libraries(MCLDMipsLDBackend ${cmake_2_8_12_PRIVATE}
  MCLDMipsTargetInfo
  MCLDCore
  MCLDFragment
  MCLDObject
  MCLDTarget
  )
