add_mcld_library(MCLDHexagonLDBackend
  HexagonAbsoluteStub.cpp
  HexagonDiagnostic.cpp
  HexagonELFDynamic.cpp
  HexagonEmulation.cpp
  HexagonGNUInfo.cpp
  HexagonGOT.cpp
  HexagonGOTPLT.cpp
  HexagonLDBackend.cpp
  HexagonPLT.cpp
  HexagonRelocator.cpp
  )

add_subdirectory(TargetInfo)

target_link_libraries(MCLDHexagonLDBackend ${cmake_2_8_12_PRIVATE}
  MCLDHexagonTargetInfo
  MCLDCore
  MCLDFragment
  MCLDObject
  MCLDTarget
  )
