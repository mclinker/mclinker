add_mcld_executable(ld.bcc
  main.cpp
  )
add_subdirectory(lib)

target_link_libraries(ld.bcc
  BCCCore
  BCCSupport
  MCLDARMLDBackend
  MCLDHexagonLDBackend
  MCLDMipsLDBackend
  MCLDX86LDBackend
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/bcc/Config/Config.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/include/bcc/Config/Config.h)

install(TARGETS ld.bcc
  RUNTIME DESTINATION bin)
