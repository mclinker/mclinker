add_mcld_library(MCLDMipsLDBackend
  MipsDiagnostic.cpp
  MipsELFDynamic.cpp
  MipsELFMCLinker.cpp
  MipsEmulation.cpp
  MipsGNUInfo.cpp
  MipsGOT.cpp
  MipsGOTPLT.cpp
  MipsLA25Stub.cpp
  MipsLDBackend.cpp
  MipsMCLinker.cpp
  MipsPLT.cpp
  MipsRelocator.cpp
  MipsTargetMachine.cpp
  )

add_subdirectory(TargetInfo)

target_link_libraries(MCLDMipsLDBackend
  MCLDMipsTargetInfo
  MCLDCore
  MCLDFragment
  MCLDObject
  MCLDTarget
  )
