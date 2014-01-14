add_mcld_library(MCLDFragment
  AlignFragment.cpp
  FillFragment.cpp
  Fragment.cpp
  FragmentRef.cpp
  NullFragment.cpp
  RegionFragment.cpp
  Relocation.cpp
  Stub.cpp
  )

target_link_libraries(MCLDFragment
  MCLDLD
  )
