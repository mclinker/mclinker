#include <mcld/Support/FileSystem.h>
#include <sys/stat.h>
#include <cerrno>
#include <stdio.h>
namespace mcld{
namespace sys{
namespace fs{

bool exists(mcld::sys::fs::FileStatus f);
bool is_directory(FileStatus f);

namespace detail{
using namespace mcld::sys::fs;

//if pLHS equal to pRHS:return true
bool is_equal(const Path& pLHS,const Path& pRHS)
{
//csmon! I'm Here
printf("%s and %s\n",pLHS.c_str(),pRHS.c_str());
  return !(strcmp(pLHS.c_str(),pRHS.c_str()));
}

bool not_found_error(int errval)
{
//csmon! I'm Here
  return errno == ENOENT || errno == ENOTDIR;
}

//FIXME:need to implement error code 
FileStatus status(const Path& p)
{
//csmon! I'm Here
  struct stat path_stat;
  if (::stat(p.c_str(), &path_stat)!= 0)
  {
    if (not_found_error(errno))
    {
      return fs::FileStatus(fs::FileNotFound);
    }
    return fs::FileStatus(fs::StatusError);
  }
  if (S_ISDIR(path_stat.st_mode))
    return fs::FileStatus(fs::DirectoryFile);
  if (S_ISREG(path_stat.st_mode))
    return fs::FileStatus(fs::RegularFile);
  if (S_ISBLK(path_stat.st_mode))
    return fs::FileStatus(fs::BlockFile);
  if (S_ISCHR(path_stat.st_mode))
    return fs::FileStatus(fs::CharacterFile);
  if (S_ISFIFO(path_stat.st_mode))
    return fs::FileStatus(fs::FifoFile);
  if (S_ISSOCK(path_stat.st_mode))
    return fs::FileStatus(fs::SocketFile);
  return fs::FileStatus(fs::TypeUnknown);
}

//  FileStatus status(const path& p, error_code* ec)
//  {
//
//    struct stat path_stat;
//    if (::stat(p.c_str(), &path_stat)!= 0)
//    {
//      if (ec != 0)                            // always report errno, even though some
//        ec->assign(errno, system_category());   // errno values are not StatusErrors
//
//      if (not_found_error(errno))
//      {
//        return fs::FileStatus(fs::FileNotFound);
//      }
//      if (ec == 0)
//        BOOST_FILESYSTEM_THROW(filesystem_error("boost::filesystem::status",
//          p, error_code(errno, system_category())));
//      return fs::FileStatus(fs::StatusError);
//    }
//    if (ec != 0) ec->clear();;
//    if (S_ISDIR(path_stat.st_mode))
//      return fs::FileStatus(fs::DirectoryFile);
//    if (S_ISREG(path_stat.st_mode))
//      return fs::FileStatus(fs::RegularFile);
//    if (S_ISBLK(path_stat.st_mode))
//      return fs::FileStatus(fs::BlockFile);
//    if (S_ISCHR(path_stat.st_mode))
//      return fs::FileStatus(fs::CharacterFile);
//    if (S_ISFIFO(path_stat.st_mode))
//      return fs::FileStatus(fs::FifoFile);
//    if (S_ISSOCK(path_stat.st_mode))
//      return fs::FileStatus(fs::SocketFile);
//    return fs::FileStatus(fs::TypeUnknown);
//  }

}
}
} 
}
