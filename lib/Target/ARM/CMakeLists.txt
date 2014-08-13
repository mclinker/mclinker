add_mcld_library(MCLDARMLDBackend
  ARMDiagnostic.cpp
  ARMELFAttributeData.cpp
  ARMELFDynamic.cpp
  ARMEmulation.cpp
  ARMGOT.cpp
  ARMLDBackend.cpp
  ARMPLT.cpp
  ARMRelocator.cpp
  ARMToARMStub.cpp
  ARMToTHMStub.cpp
  THMToARMStub.cpp
  THMToTHMStub.cpp
  )

add_subdirectory(TargetInfo)

target_link_libraries(MCLDARMLDBackend ${cmake_2_8_12_PRIVATE}
  MCLDARMTargetInfo
  MCLDCore
  MCLDFragment
  MCLDObject
  MCLDTarget
  )
