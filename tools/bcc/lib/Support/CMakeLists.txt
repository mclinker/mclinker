add_mcld_library(BCCSupport
  Initialization.cpp
  LinkerConfig.cpp
  TargetLinkerConfigs.cpp
  )

target_link_libraries(BCCSupport
  MCLDADT
  )
