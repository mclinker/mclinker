add_mcld_library(MCLDSupport
  CommandLine.cpp
  Demangle.cpp
  Directory.cpp
  FileHandle.cpp
  FileOutputBuffer.cpp
  FileSystem.cpp
  LEB128.cpp
  MemoryArea.cpp
  MemoryAreaFactory.cpp
  MsgHandling.cpp
  Path.cpp
  raw_ostream.cpp
  RealPath.cpp
  SystemUtils.cpp
  Target.cpp
  TargetRegistry.cpp
  Unix/FileSystem.inc
  Unix/PathV3.inc
  Unix/System.inc
  Windows/FileSystem.inc
  Windows/PathV3.inc
  Windows/System.inc
  )

target_link_libraries(MCLDSupport ${cmake_2_8_12_PRIVATE}
  MCLDLD
  )
