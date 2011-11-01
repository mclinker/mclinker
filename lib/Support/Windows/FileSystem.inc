//===- FileSystem.inc -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <string>

namespace mcld{
namespace sys{
namespace fs{
namespace detail{

std::string static_library_extension = ".lib";
std::string shared_library_extension = ".dll";
std::string executable_extension = ".exe";
std::string relocatable_extension = ".obj";
std::string assembly_extension = ".s";
std::string bitcode_extension = ".bc";


} // namespace of detail
} // namespace of fs
} // namespace of sys
} // namespace of mcld

