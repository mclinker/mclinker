add_mcld_library(MCLDX86LDBackend
  X86Diagnostic.cpp
  X86ELFDynamic.cpp
  X86Emulation.cpp
  X86GOT.cpp
  X86GOTPLT.cpp
  X86LDBackend.cpp
  X86PLT.cpp
  X86Relocator.cpp
  )

add_subdirectory(TargetInfo)

target_link_libraries(MCLDX86LDBackend ${cmake_2_8_12_PRIVATE}
  MCLDX86TargetInfo
  MCLDCore
  MCLDFragment
  MCLDObject
  MCLDTarget
  )
