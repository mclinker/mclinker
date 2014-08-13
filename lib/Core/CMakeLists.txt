add_mcld_library(MCLDCore
  AttributeOption.cpp
  Environment.cpp
  GeneralOptions.cpp
  InputTree.cpp
  IRBuilder.cpp
  Linker.cpp
  LinkerConfig.cpp
  LinkerScript.cpp
  Module.cpp
  TargetOptions.cpp
  )

target_link_libraries(MCLDCore ${cmake_2_8_12_PRIVATE}
  MCLDADT
  MCLDMC
  MCLDObject
  MCLDAArch64LDBackend
  MCLDARMLDBackend
  MCLDHexagonLDBackend
  MCLDMipsLDBackend
  MCLDX86LDBackend
  )
