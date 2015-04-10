add_llvm_library(MCLDX86LDBackend
  X86Diagnostic.cpp
  X86ELFDynamic.cpp
  X86Emulation.cpp
  X86GOT.cpp
  X86GOTPLT.cpp
  X86LDBackend.cpp
  X86PLT.cpp
  X86Relocator.cpp
  LINK_LIBS
    MCLDX86TargetInfo
    MCLDCore
    MCLDFragment
    MCLDObject
    MCLDTarget
  )

add_subdirectory(TargetInfo)
