include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../include )

add_mcld_library(LiteOptions
  DynamicSectionOptions.cpp
  OptimizationOptions.cpp
  OutputFormatOptions.cpp
  PositionalOptions.cpp
  PreferenceOptions.cpp
  ScriptOptions.cpp
  SearchPathOptions.cpp
  SymbolOptions.cpp
  TargetControlOptions.cpp
  TripleOptions.cpp
  )
