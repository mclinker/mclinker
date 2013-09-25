add_mcld_library(MCLDTarget
  ELFAttribute.cpp
  ELFAttributeData.cpp
  ELFAttributeValue.cpp
  ELFDynamic.cpp
  ELFEmulation.cpp
  ELFMCLinker.cpp
  GNUInfo.cpp
  GNULDBackend.cpp
  GOT.cpp
  OutputRelocSection.cpp
  PLT.cpp
  TargetLDBackend.cpp
  )

add_subdirectory(ARM)
add_subdirectory(Hexagon)
add_subdirectory(Mips)
add_subdirectory(X86)

target_link_libraries(MCLDTarget
  MCLDCodeGen
  MCLDLD
  )
