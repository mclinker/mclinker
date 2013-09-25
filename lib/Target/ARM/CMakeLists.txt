add_mcld_library(MCLDARMLDBackend
  ARMDiagnostic.cpp
  ARMELFAttributeData.cpp
  ARMELFDynamic.cpp
  ARMELFMCLinker.cpp
  ARMEmulation.cpp
  ARMGOT.cpp
  ARMLDBackend.cpp
  ARMMCLinker.cpp
  ARMPLT.cpp
  ARMRelocator.cpp
  ARMTargetMachine.cpp
  ARMToARMStub.cpp
  ARMToTHMStub.cpp
  THMToARMStub.cpp
  THMToTHMStub.cpp
  )

add_subdirectory(TargetInfo)

target_link_libraries(MCLDARMLDBackend
  MCLDARMTargetInfo
  MCLDCore
  MCLDFragment
  MCLDObject
  MCLDTarget
  )
