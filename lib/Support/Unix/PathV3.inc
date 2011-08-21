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
#include <sys/stat.h>
#include <cerrno>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <string>

using namespace std;

namespace mcld{
namespace sys{
namespace fs{
namespace detail{

static void read_dir(DIR* pDir, std::string& pOutFilename)
{
  dirent *cur_dir = ::readdir(pDir);
  if (0 == cur_dir && 0 != errno) {
    // FIXME: use error_code
    llvm::report_fatal_error("failed to read the directory");
  }

  // idx does not stay at the end, but all elements had beed put into cache.
  if (0 == cur_dir) {
    return;
  }
  
  llvm::StringRef name(cur_dir->d_name, strlen(cur_dir->d_name));
  if ((name.size() == 1 && name[0] == '.') ||
      (name.size() == 2 && name[0] == '.' && name[1] == '.'))
    return read_dir(pDir, pOutFilename);

  // find a new directory
  pOutFilename.assign(name.data(), name.size());
  return;
}

string canonical_form(const Path& pPath)
{
  //initial PathResult
  string PathResult;
  if(pPath.is_from_root())
  {
    PathResult = "/";
  }
  else
  {
    char TempPath[100];//FIXME:if getcwd() does not need TempPath[], remove it
    PathResult = getcwd(TempPath, 100);
    PathResult += '/';
  }
  //check slash num and position
  size_t SepNum = 0;
  size_t SepPos[100];//FIXME:use array or strmap
  for(int i=0;i < PathResult.size();++i)
  {
    if('/' == PathResult[i])
    {
      SepPos[SepNum] = i;
      SepNum++;
    }
  }
  //if go parent:SepNum--
  for(int i = 0;i<pPath.native().size();++i)
  {
    switch(pPath.native()[i]){
      case '/'://FIXME use member function  m_erase_redundant_separator
      {
        for(;pPath.native()[i] == '/';++i)
        {
          //do nothing
        }
        if(i != 1 && PathResult[PathResult.size()-1] != '/')
        {
          PathResult += "/";
        }
        i--;
        break;
      }
      case '.':
      {
        int DotNum=0;
        for(;pPath.native()[i] == '.';++i){DotNum++;}
        if(DotNum>2)
        {
          cerr<<"illegal path name: "<<pPath.native()<<endl;
        }
        else if(2 == DotNum)
        {
          SepNum=SepNum - 1;
          PathResult.erase(SepPos[SepNum -1 ] + 1,
                           SepPos[SepNum] - SepPos[SepNum - 1]);
        }
        else if(1 == DotNum)
        {
          //do nothing
        }
        else
        {
          cerr<<"unknown error when handle dot of path: "<<pPath.native()<<endl;
        }
        //i--;//skip '/' because it does not matter
        break;
      }
      default:
      {
        int CharNum=0;
        for(;i < pPath.native().size()&&pPath.native()[i] != '/';++i)
        {
          CharNum++;
        }
        PathResult.append(pPath.native(),i - CharNum,CharNum);
        SepPos[SepNum]= SepPos[SepNum - 1] + CharNum + 1;
        SepNum++;
        i--;
        break;
      }
    }
  }
  if(PathResult[PathResult.size() - 1] != '/'&& !is_directory(pPath))
  {
    PathResult += "/";
  }
  return PathResult;//FIXME:use return or use reference
}

//turn path into canonical form
void canonical_form(mcld::sys::fs::Path& pPath)
{
  //erase_redundant_sep
  for(int i = 0;i<pPath.native().size();++i) {
    if(pPath.native()[i]=='/'){
      pPath.m_erase_redundant_separator(i);
    }
  }

  //initial PathResult
  string PathResult;
  if(pPath.is_from_root()) {
    PathResult = "/";
  }
  else {
    char TempPath[100];//FIXME:if getcwd() does not need TempPath[], remove it
    PathResult = getcwd(TempPath, 100);
    PathResult += '/';
  }

  //check slash num and position
  size_t SepNum = 0;
  size_t SepPos[100];//FIXME:use array or strmap
  for(int i=0;i < PathResult.size();++i) {
    if('/' == PathResult[i]) {
      SepPos[SepNum] = i;
      SepNum++;
    }
  }
  //if go parent:SepNum--
  for(int i = 0;i<pPath.native().size();++i) {
    switch(pPath.native()[i]) {
      case '/'://FIXME use member function  m_erase_redundant_separator
      {
        PathResult += "/";
        break;
      }
      case '.':
      {
        int DotNum=0;
        for(;pPath.native()[i] == '.';++i){DotNum++;}
        if(DotNum>2)
        {
          cerr<<"illegal path name: "<<pPath.native()<<endl;
        }
        else if(2 == DotNum)
        {
          SepNum=SepNum - 1;
          PathResult.erase(SepPos[SepNum -1 ] + 1,
                           SepPos[SepNum] - SepPos[SepNum - 1]);
        }
        else if(1 == DotNum)
        {
          //do nothing
        }
        else
        {
          cerr<<"unknown error when handle dot of path: "<<pPath.native()<<endl;
        }
        //i--;skip '/' or it is end of path
        break;
      }
      default:
      {
        int CharNum=0;
        for(;i < pPath.native().size()&&pPath.native()[i] != '/';++i)
        {
          CharNum++;
        }
        PathResult.append(pPath.native(),i - CharNum,CharNum);
        SepPos[SepNum]= SepPos[SepNum - 1] + CharNum + 1;
        SepNum++;
        i--;
        break;
      }
    }
  }
  pPath.assign(PathResult);
  pPath.m_append_separator_if_needed(); 
}

//if pLHS equal to pRHS, return true FIXME:not use
//bool is_equal(Path& pLHS,Path& pRHS)
//{
//  return (canonical_form(pLHS)==canonical_form(pRHS));
//}

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

/// directory_iterator_increment - increment function implementation
//
//  iterator will call this function in two situations:
//  1. All elements have been put into cache, and iterator stays at the end
//     of cache. (a real end)
//  2. Some but not all elements had beed put into cache, and we stoped.
//     An iterator now is staying at the end of cache. (a temporal end)
void directory_iterator_increment(DirIterator& pIter)
{
  if (0 == pIter.m_pParent || 0 == pIter.m_pIdx) // situation 1, empty iterator
    return;

  if (0 == pIter.m_pPath) // situation 1, we are at real end
    return;

  // try to read more one.
  dirent *cur_dir = ::readdir(reinterpret_cast<DIR *>(pIter.m_pParent->m_Handler));
  if (0 == cur_dir && 0 != errno) {
    // FIXME: use error_code
    llvm::report_fatal_error("failed to read the directory");
  }
  // idx does not stay at the end, but all elements had beed put into cache.
  if (0 == cur_dir) {
    pIter.m_pPath = 0;
    return;
  }
  
  llvm::StringRef name(cur_dir->d_name, strlen(cur_dir->d_name));
  if ((name.size() == 1 && name[0] == '.') ||
      (name.size() == 2 && name[0] == '.' && name[1] == '.'))
    return directory_iterator_increment(pIter);

  // find a new directory
  pIter.m_pPath = &pIter.m_pParent->m_Cache.GetOrCreateValue(name).getValue();
  pIter.m_pPath->assign(name.data(), name.size());
  new(pIter.m_pIdx) DirIterator::DirCache::iterator(pIter.m_pParent->m_Cache.end());
}

void open_dir(Directory& pDir)
{
  DIR* dip = opendir(pDir.path().c_str());
  if (0 == dip)
    llvm::report_fatal_error(std::string("Can't open directory: ")+
                       pDir.path().native());
  pDir.m_Handler = reinterpret_cast<intptr_t>(dip);
  // read one entry for advance the end element of the cache.
  std::string pEntry;
  read_dir(dip, pEntry);

  // find a new directory
  Path& path = pDir.m_Cache.GetOrCreateValue(pEntry).getValue();
  path.assign(pEntry);
}

void close_dir(Directory& pDir)
{
  if (pDir.m_Handler)
    closedir(reinterpret_cast<DIR *>(pDir.m_Handler));
  pDir.m_Handler = 0;
}

} // namespace of detail
} // namespace of fs
} // namespace of sys
} // namespace of mcld

