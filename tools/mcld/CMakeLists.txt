include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

add_mcld_executable(ld.mcld
  main.cpp
  )
add_subdirectory(lib)

target_link_libraries(ld.mcld
  LiteOptions
  MCLDADT
  MCLDAArch64LDBackend
  MCLDARMLDBackend
  MCLDHexagonLDBackend
  MCLDMipsLDBackend
  MCLDX86LDBackend
)

install(TARGETS ld.mcld
  RUNTIME DESTINATION bin)
