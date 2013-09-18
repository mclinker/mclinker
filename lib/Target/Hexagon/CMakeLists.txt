add_mcld_library(MCLDHexagonLDBackend
  HexagonAbsoluteStub.cpp
  HexagonDiagnostic.cpp
  HexagonELFDynamic.cpp
  HexagonELFMCLinker.cpp
  HexagonEmulation.cpp
  HexagonGNUInfo.cpp
  HexagonGOT.cpp
  HexagonGOTPLT.cpp
  HexagonLDBackend.cpp
  HexagonMCLinker.cpp
  HexagonPLT.cpp
  HexagonRelocator.cpp
  HexagonTargetMachine.cpp
  )

add_subdirectory(TargetInfo)

target_link_libraries(MCLDHexagonLDBackend
  MCLDHexagonTargetInfo
  MCLDCore
  MCLDFragment
  MCLDObject
  MCLDTarget
  )
