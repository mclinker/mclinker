add_mcld_library(MCLDObject
  ObjectBuilder.cpp
  ObjectLinker.cpp
  SectionMap.cpp
  )

target_link_libraries(MCLDObject ${cmake_2_8_12_PRIVATE}
  MCLDFragment
  MCLDLD
  MCLDScript
  )
