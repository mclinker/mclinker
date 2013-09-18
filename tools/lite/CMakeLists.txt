include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

add_mcld_executable(ld.lite
  main.cpp
  )
add_subdirectory(lib)

target_link_libraries(ld.lite
  LiteOptions
  MCLDADT
  MCLDARMLDBackend
  MCLDHexagonLDBackend
  MCLDMipsLDBackend
  MCLDX86LDBackend
)

install(TARGETS ld.lite
  RUNTIME DESTINATION bin)
