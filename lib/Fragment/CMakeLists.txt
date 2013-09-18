add_mcld_library(MCLDFragment
  AlignFragment.cpp
  FGNode.cpp
  FillFragment.cpp
  Fragment.cpp
  FragmentGraph.cpp
  FragmentRef.cpp
  NullFragment.cpp
  RegionFragment.cpp
  Relocation.cpp
  Stub.cpp
  )

target_link_libraries(MCLDFragment
  MCLDLD
  )
