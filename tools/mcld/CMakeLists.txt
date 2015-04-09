include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )
set(LLVM_TARGET_DEFINITIONS Options.td)
tablegen(LLVM Options.inc -gen-opt-parser-defs)
add_public_tablegen_target(DriverOptionsTableGen)

add_mcld_executable(ld.mcld
  Main.cpp
  )

target_link_libraries(ld.mcld
  MCLDADT
  MCLDAArch64LDBackend
  MCLDARMLDBackend
  MCLDHexagonLDBackend
  MCLDMipsLDBackend
  MCLDX86LDBackend
  LLVMOption
)

install(TARGETS ld.mcld
  RUNTIME DESTINATION bin)

add_dependencies(ld.mcld DriverOptionsTableGen)
