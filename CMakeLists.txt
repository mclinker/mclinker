# If we are not building as a part of LLVM, build mcld as a standalone project,
# using LLVM as an external library.



if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(mcld)
  cmake_minimum_required(VERSION 2.8)

  set(MCLD_PATH_TO_LLVM_SOURCE "" CACHE PATH
    "Path to LLVM source code. Not necessary if using an installed LLVM.")
  set(MCLD_PATH_TO_LLVM_BUILD "" CACHE PATH
    "Path to the directory where LLVM was built or installed.")

  if (MCLD_PATH_TO_LLVM_SOURCE)
    if (NOT EXISTS "${MCLD_PATH_TO_LLVM_SOURCE}/cmake/config-ix.cmake")
      message(FATAL_ERROR "Please set MCLD_PATH_TO_LLVM_SOURCE to the root "
              "directory of LLVM source code.")
    else()
      get_filename_component(LLVM_MAIN_SRC_DIR ${MCLD_PATH_TO_LLVM_SOURCE}
                             ABSOLUTE)
      list(APPEND CMAKE_MODULE_PATH "${LLVM_MAIN_SRC_DIR}/cmake/modules")
    endif()
  endif()

  list(APPEND CMAKE_MODULE_PATH "${MCLD_PATH_TO_LLVM_BUILD}/share/llvm/cmake")

  get_filename_component(PATH_TO_LLVM_BUILD ${MCLD_PATH_TO_LLVM_BUILD}
                         ABSOLUTE)

  include(AddLLVM)
  include(TableGen)
  include("${MCLD_PATH_TO_LLVM_BUILD}/share/llvm/cmake/LLVMConfig.cmake")
  include(HandleLLVMOptions)

  set(PACKAGE_VERSION "${LLVM_PACKAGE_VERSION}")

  set(LLVM_MAIN_INCLUDE_DIR "${LLVM_MAIN_SRC_DIR}/include")
  set(LLVM_BINARY_DIR ${CMAKE_BINARY_DIR})

  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  include_directories("${PATH_TO_LLVM_BUILD}/include"
                      "${LLVM_MAIN_INCLUDE_DIR}")
  link_directories("${PATH_TO_LLVM_BUILD}/lib")

  if( EXISTS "${MCLD_PATH_TO_LLVM_BUILD}/bin/llvm-tblgen${CMAKE_EXECUTABLE_SUFFIX}" )
    set(LLVM_TABLEGEN_EXE "${PATH_TO_LLVM_BUILD}/bin/llvm-tblgen${CMAKE_EXECUTABLE_SUFFIX}")
  else()
    # FIXME: This is an utter hack.
    set(LLVM_TABLEGEN_EXE "${PATH_TO_LLVM_BUILD}/bin/Debug/llvm-tblgen${CMAKE_EXECUTABLE_SUFFIX}")
  endif()

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

  set(MCLD_BUILT_STANDALONE 1)
endif()

set(MCLD_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(MCLD_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(MCLD_DEFAULT_TARGET_TRIPLE "${LLVM_HOST_TRIPLE}" CACHE STRING
  "Default target for which LLVM will generate code." )

################################################################################
#  Versioning, see VERSION
  set(MAJOR 2)
  set(MINOR 9)
  set(MICRO 0)
  set(BUILD dev)
  set(DEV_VERSION ${MAJOR}.${MINOR}.${MICRO}-${BUILD})
  set(PKG_VERSION ${MAJOR}.${MINOR}.${MICRO}-${BUILD})
  set(MCLD_VERSION ${DEV_VERSION})
  message(STATUS "MCLD version: " ${MCLD_VERSION})

set(MCLD_ALL_TARGETS
  AArch64
  ARM
  Hexagon
  Mips
  X86
  )

set(TARGET_BUILD all
    CACHE STRING "MCLD Target(Targets) to build")
if( TARGET_BUILD STREQUAL "all" )
  set( TARGET_BUILD ${MCLD_ALL_TARGETS} )
  set ( DEFAULT_ALONE_TARGET "X86")
endif()

# set MCLD_ENUM_TARGETS="MCLD_TARGET($target_to_build)
set (MCLD_ENUM_TARGETS "")
foreach(t ${TARGET_BUILD})
  set (MCLD_ENUM_TARGETS ${MCLD_ENUM_TARGETS} "MCLD_TARGET(${t}) \n")
  set (MCLD_ENUM_LINKERS ${MCLD_ENUM_LINKERS} "MCLD_LINKER(${t}) \n")
endforeach()
message(
  "enum == " ${MCLD_ENUM_TARGETS}
  "enum == " ${MCLD_ENUM_LINKERS}
)

if(WIN32)
  if(CYGWIN)
    set(MCLD_ON_WIN32 0)
    set(MCLD_ON_UNIX 1)
  else(CYGWIN)
    set(MCLD_ON_WIN32 1)
    set(MCLD_ON_UNIX 0)
  endif(CYGWIN)
else(WIN32)
  if(UNIX)
    set(MCLD_ON_WIN32 0)
    set(MCLD_ON_UNIX 1)
  else(UNIX)
    MESSAGE(SEND_ERROR "Unable to determine platform")
  endif(UNIX)
endif(WIN32)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not allowed. CMake would overwrite "
"the makefiles distributed with LLVM. Please create a directory and run cmake "
"from there, passing the path to this source directory as the last argument. "
"This process created the file `CMakeCache.txt' and the directory "
"`CMakeFiles'. Please delete them.")
endif()

list (APPEND CMAKE_MODULE_PATH "${MCLD_SOURCE_DIR}/cmake/modules")

option(MCLD_USE_VTUNE
       "Enable VTune user task tracking."
       OFF)
if (MCLD_USE_VTUNE)
  find_package(VTune)
  if (VTUNE_FOUND)
    include_directories(${VTune_INCLUDE_DIRS})
    list(APPEND LLVM_COMMON_LIBS ${VTune_LIBRARIES})
    add_definitions(-DMCLD_HAS_VTUNE)
  endif()
endif()

# MCLD requires c++11 to build. Make sure that we have a compiler and standard
# library combination that can do that.
if (MSVC11)
  # Do nothing, we're good.
elseif (NOT MSVC)
  # gcc and clang require the -std=c++0x or -std=c++11 flag.
#  if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU|Clang" AND
#      NOT ("${CMAKE_CXX_FLAGS}" MATCHES ".*-std=(c|gnu)\\+\\+(0x|11).*"))
#    message(FATAL_ERROR
#      "MCLD requires c++11. Clang and gcc require -std=c++0x or -std=c++11 to "
#      "enter this mode. Please set CMAKE_CXX_FLAGS accordingly.")
#  endif()
else()
  message(FATAL_ERROR "The selected compiler does not support c++11 which is "
          "required to build MCLD.")
endif()
# Configure the three MCLD configuration header files.
configure_file(
  ${MCLD_SOURCE_DIR}/include/mcld/Config/Config.h.cmake
  ${MCLD_BINARY_DIR}/include/mcld/Config/Config.h)
configure_file(
  ${MCLD_SOURCE_DIR}/include/mcld/Config/Linkers.def.cmake
  ${MCLD_BINARY_DIR}/include/mcld/Config/Linkers.def
  )
configure_file(
  ${MCLD_SOURCE_DIR}/include/mcld/Config/Targets.def.cmake
  ${MCLD_BINARY_DIR}/include/mcld/Config/Targets.def
  )



macro(add_mcld_library name)
  llvm_process_sources(srcs ${ARGN})
  if (MSVC_IDE OR XCODE)
    string(REGEX MATCHALL "/[^/]+" split_path ${CMAKE_CURRENT_SOURCE_DIR})
    list(GET split_path -1 dir)
    file(GLOB_RECURSE headers
      ../../include/mcld${dir}/*.h)
    set(srcs ${srcs} ${headers})
  endif()
  if (MODULE)
    set(libkind MODULE)
  elseif (SHARED_LIBRARY)
    set(libkind SHARED)
  else()
    set(libkind)
  endif()

  add_library(${name} ${libkind} ${srcs})
  llvm_update_compile_flags(${name} ${srcs})
  if (LLVM_COMMON_DEPENDS)
    add_dependencies(${name} ${LLVM_COMMON_DEPENDS})
  endif()

  target_link_libraries(${name} ${LLVM_USED_LIBS})
  llvm_config(${name} ${LLVM_LINK_COMPONENTS})
  target_link_libraries(${name} ${LLVM_COMMON_LIBS})

  install(TARGETS ${name}
    LIBRARY DESTINATION lib${LLVM_LIBDIR_SUFFIX}
    ARCHIVE DESTINATION lib${LLVM_LIBDIR_SUFFIX})
  set_target_properties(${name} PROPERTIES FOLDER "MCLD libraries")
endmacro(add_mcld_library)

macro(add_mcld_executable name)
  add_llvm_executable(${name} ${ARGN})
  set_target_properties(${name} PROPERTIES FOLDER "MCLD executables")
endmacro(add_mcld_executable)


set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(BEFORE
  ${CMAKE_CURRENT_BINARY_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}/tools/ld.mcld/include
  ${CMAKE_CURRENT_SOURCE_DIR}/tools/ld.mcld/include
  )
link_directories("${PATH_TO_LLVM_BUILD}/lib")

set(LLVM_LINK_COMPONENTS ${LLVM_TARGETS_TO_BUILD} bitreader asmparser)

install(DIRECTORY include/
  DESTINATION include
  FILES_MATCHING
  PATTERN "*.h"
  PATTERN ".svn" EXCLUDE
  )

add_subdirectory(lib)
add_subdirectory(tools)

