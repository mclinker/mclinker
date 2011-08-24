/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Chun-Hung Lu <chun-hung.lu@mediatek.com>                                *
 ****************************************************************************/
#include <llvm/Support/ErrorHandling.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/Support/Directory.h>
#include <mcld/Support/Path.h>
#include <sys/stat.h>
#include <cerrno>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <string>
#include <stack>

namespace mcld{
namespace sys{
namespace fs{
namespace detail{

const char          separator = '/';
const char          preferred_separator = '/';

// return the last charactor being handled.
size_t canonicalize(std::string& pathname)
{
  // Variable Index //
  // SepTable - stack of result separators
  // LR(1) Algorithm //
  // traverse pPathName
  //   if we meet '//', '///', '////', ...
  //     -> ignore it
  //     -> push current into stack
  //     -> jump to the next not '/'
  //   if we meet '/./'
  //     -> ignore
  //     -> jump to the next not '/'
  //   if we meet '/../'
  //     -> pop previous position of '/' P
  //     -> erase P+1 to now
  //   if we meet other else
  //     -> go go go
  //   if we meet '/.../', '/..../', ... -> illegal
  if (pathname.empty())
    return 0;

  int handler = 0;
  stack<int> slash_stack;
  slash_stack.push(-1);
  while (handler < pathname.size()) {
    if (separator == pathname[handler]) { // handler = 1st '/'
      int next = handler + 1;
      if (next >= pathname.size())
        return handler;
      switch(pathname[next]) { // next = handler + 1;
        case separator: { // '//'
          while (next < pathname.size() && separator == pathname[next])
            ++next;
          // next is the last not '/'
          pathname.erase(handler, next - handler - 1);
          // handler is the first '/'
          slash_stack.push(handler);
          break;
        }
        case '.': { // '/.'
          ++next; // next = handler + 2
          if (next >= pathname.size()) // '/.'
            return handler;
          switch (pathname[next]) {
            case separator: { // '/./'
              pathname.erase(handler, 2);
              break;
            }
            case '.': { // '/..'
              ++next; // next = handler + 3;
              if (next >= pathname.size()) // '/..?'
                return handler;
              switch(pathname[next]) {
                case separator: { // '/../'
                  handler = slash_stack.top();
                  slash_stack.pop();
                  pathname.erase(handler+1, next-handler);
                  if (-1 == handler) {
                    slash_stack.push(-1);
                    handler = pathname.find_first_of(separator, handler);
                  }
                  break;
                }
                case '.': { // '/...', illegal
                  return handler;
                  break;
                }
                default : { // '/..a'
                  slash_stack.push(handler);
                  handler = pathname.find_first_of(separator, handler+3);
                  break;
                }
              }
              break;
            }
            default : { // '/.a'
              slash_stack.push(handler);
              handler = pathname.find_first_of(separator, handler+2);
              break;
            }
          }
          break;
        }
        default : { // '/a
          slash_stack.push(handler);
          handler = pathname.find_first_of(separator, handler+1);
          break;
        }
      }
    }
    else {
      handler = pathname.find_first_of(separator, handler);
    }
  }
}

bool not_found_error(int perrno)
{
  return perrno == ENOENT || perrno == ENOTDIR;
}

void status(const Path& p, FileStatus& pFileStatus)
{
  struct stat path_stat;
  if(stat(p.c_str(), &path_stat)!= 0)
  {
    if(not_found_error(errno))
    {
      pFileStatus.setType(FileNotFound);
    }
    else
      pFileStatus.setType(StatusError);
  }
  else if(S_ISDIR(path_stat.st_mode))
    pFileStatus.setType(DirectoryFile);
  else if(S_ISREG(path_stat.st_mode))
    pFileStatus.setType(RegularFile);
  else if(S_ISBLK(path_stat.st_mode))
    pFileStatus.setType(BlockFile);
  else if(S_ISCHR(path_stat.st_mode))
    pFileStatus.setType(CharacterFile);
  else if(S_ISFIFO(path_stat.st_mode))
    pFileStatus.setType(FifoFile);
  else if(S_ISSOCK(path_stat.st_mode))
    pFileStatus.setType(SocketFile);
  else
    pFileStatus.setType(TypeUnknown);
}

void symlink_status(const Path& p, FileStatus& pFileStatus)
{
  struct stat path_stat;
  if(lstat(p.c_str(), &path_stat)!= 0)
  {
    if(errno == ENOENT || errno == ENOTDIR) // these are not errors
    {
      pFileStatus.setType(FileNotFound);
    }
    else
      pFileStatus.setType(StatusError);
  }
  if(S_ISREG(path_stat.st_mode))
    pFileStatus.setType(RegularFile);
  if(S_ISDIR(path_stat.st_mode))
    pFileStatus.setType(DirectoryFile);
  if(S_ISLNK(path_stat.st_mode))
    pFileStatus.setType(SymlinkFile);
  if(S_ISBLK(path_stat.st_mode))
    pFileStatus.setType(BlockFile);
  if(S_ISCHR(path_stat.st_mode))
    pFileStatus.setType(CharacterFile);
  if(S_ISFIFO(path_stat.st_mode))
    pFileStatus.setType(FifoFile);
  if(S_ISSOCK(path_stat.st_mode))
    pFileStatus.setType(SocketFile);
  else
    pFileStatus.setType(TypeUnknown);
}

/// read_dir - return true if we read one entry
//  @return value -1: read error
//                 0: read the end
//                 1: success
static int read_dir(intptr_t& pDir, std::string& pOutFilename)
{
  dirent *cur_dir = ::readdir(reinterpret_cast<DIR*>(pDir));
  if (0 == cur_dir && 0 != errno)
    return -1;

  // idx does not stay at the end, but all elements had beed put into cache.
  if (NULL == cur_dir) {
    return 0;
  }

  llvm::StringRef name(cur_dir->d_name, strlen(cur_dir->d_name));
  if ((name.size() == 1 && name[0] == '.') ||
      (name.size() == 2 && name[0] == '.' && name[1] == '.'))
    return read_dir(pDir, pOutFilename);

  // find a new directory
  pOutFilename.append(name.data(), name.size());
  return 1;
}

/// directory_iterator_increment - increment function implementation
//
//  iterator will call this function in two situations:
//  1. All elements have been put into cache, and iterator stays at the end
//     of cache. (a real end)
//  2. Some but not all elements had beed put into cache, and we stoped.
//     An iterator now is staying at the end of cache. (a temporal end)
Directory::PathCache::iterator bring_one_into_cache(DirIterator& pIter)
{
  std::string path(pIter.m_pParent->m_Path.native());
  switch (read_dir(pIter.m_pParent->m_Handler, path)) {
  case 1:
    return pIter.m_pParent->m_Cache.GetOrCreateValue(path, new Path(path));
  case 0:// meet real end
    return pIter.m_pParent->m_Cache.end();
  case -1:
    llvm::report_fatal_error(std::string("Can't read directory: ")+
                             pIter.m_pParent->path().native());
  }
}

void open_dir(Directory& pDir)
{
  pDir.m_Handler = reinterpret_cast<intptr_t>(opendir(pDir.path().c_str()));
  if (NULL == pDir.m_Handler)
    llvm::report_fatal_error(std::string("Can't open directory: ")+
                       pDir.path().native());
  // read one entry for advance the end element of the cache.
  std::string path;
  switch (read_dir(pDir.m_Handler, path)) {
  case 1:
    // find a new directory
    pDir.m_Cache.GetOrCreateValue(path, new Path(path));
    return;
  case 0:
    return;
  case -1:
    llvm::report_fatal_error(std::string("Can't read directory: ")+
                             pDir.path().native());
  }
}

void close_dir(Directory& pDir)
{
  if (pDir.m_Handler)
    closedir(reinterpret_cast<DIR *>(pDir.m_Handler));
  pDir.m_Handler = NULL;
}

void get_pwd(std::string& pPWD)
{
  char* pwd = get_current_dir_name();
  pPWD.assign(pwd);
  free(pwd);
}

} // namespace of detail
} // namespace of fs
} // namespace of sys
} // namespace of mcld

