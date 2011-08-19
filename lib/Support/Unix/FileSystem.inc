/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Chun-Hung Lu <chun-hung.lu@mediatek.com>                                *
 ****************************************************************************/
#include <string>

namespace mcld{
namespace sys{
namespace fs{
namespace detail{

std::string static_library_extension = ".a";
std::string shared_library_extension = ".so";
std::string executable_extension = "";
std::string relocatable_extension = ".o";
std::string assembly_extension = ".s";
std::string bitcode_extension = ".bc";


} // namespace of detail
} // namespace of fs
} // namespace of sys
} // namespace of mcld

