set(LLVM_LINK_COMPONENTS ${LLVM_TARGETS_TO_BUILD} irreader)

add_mcld_executable(ld.mcld
  main.cpp
  )

target_link_libraries(ld.mcld
  MCLDADT
  MCLDARMLDBackend
  MCLDHexagonLDBackend
  MCLDMipsLDBackend
  MCLDX86LDBackend
  )

install(TARGETS ld.mcld
  RUNTIME DESTINATION bin)
