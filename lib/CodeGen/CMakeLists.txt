add_mcld_library(MCLDCodeGen
  MCLDTargetMachine.cpp
  MCLinker.cpp
  )

target_link_libraries(MCLDCodeGen
  MCLDCore
  MCLDMC
  )
