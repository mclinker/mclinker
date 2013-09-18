add_mcld_library(MCLDX86LDBackend
  X86Diagnostic.cpp
  X86ELFDynamic.cpp
  X86ELFMCLinker.cpp
  X86Emulation.cpp
  X86GOT.cpp
  X86GOTPLT.cpp
  X86LDBackend.cpp
  X86MCLinker.cpp
  X86PLT.cpp
  X86Relocator.cpp
  X86TargetMachine.cpp
  )

add_subdirectory(TargetInfo)

target_link_libraries(MCLDX86LDBackend
  MCLDX86TargetInfo
  MCLDCore
  MCLDFragment
  MCLDObject
  MCLDTarget
  )
