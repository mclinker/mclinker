add_mcld_library(MCLDTarget
  ELFAttribute.cpp
  ELFAttributeData.cpp
  ELFAttributeValue.cpp
  ELFDynamic.cpp
  ELFEmulation.cpp
  GNUInfo.cpp
  GNULDBackend.cpp
  GOT.cpp
  OutputRelocSection.cpp
  PLT.cpp
  TargetLDBackend.cpp
  )

add_subdirectory(AArch64)
add_subdirectory(ARM)
add_subdirectory(Hexagon)
add_subdirectory(Mips)
add_subdirectory(X86)

target_link_libraries(MCLDTarget ${cmake_2_8_12_PRIVATE}
  MCLDLD
  )
