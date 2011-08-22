/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Chun-Hung Lu <chun-hung.lu@mediatek.com>                                *
 ****************************************************************************/
#include <mcld/Support/Path.h>
#include <llvm/ADT/StringRef.h>
#include <mcld/Support/FileSystem.h>
#include <locale>
#include <stdio.h>
#include <string.h>

using namespace mcld;
using namespace mcld::sys::fs;

namespace { // anonymous
#ifdef LLVM_ON_WIN32
  const wchar_t       separator = L'\\';
  const wchar_t       preferred_separator = L'\\';
#else
  const char          separator = '/';
  const char          preferred_separator = '/';
#endif

} // namespace of anonymous

//===--------------------------------------------------------------------===//
// Path
Path::Path()
  : m_PathName() {
}

Path::Path(const Path::ValueType* s )
  : m_PathName(s) {
}

Path::Path(const Path::StringType &s )
  : m_PathName(s) {
}

Path::~Path()
{
}

Path& Path::assign(const Path::StringType &s)
{
  m_PathName.assign(s);
  return *this;
}

Path& Path::assign(const Path::ValueType* s, unsigned int length)
{
  if (0 == s || 0 == length)
    assert(0 && "assign a null or empty string to Path");
  m_PathName.assign(s, length);
  return *this;
}

//a,/b a/,b a/,b/ a,b is a/b
Path& Path::append(const Path& pPath)
{
  //first path is a/,second path is /b 
  if(m_PathName[m_PathName.length()-1] == separator &&
     pPath.native()[0] == separator) { 
    unsigned int old_size = m_PathName.size()-1;
    unsigned int new_size = old_size + pPath.native().size();

    m_PathName.resize(new_size);
    strcpy(const_cast<char*>(m_PathName.data()+old_size), pPath.native().data());
  }
  //first path is a,second path is b
  else if(this->string()[this->native().size()-1] != separator &&
          pPath.string()[0] != separator) {
    m_PathName.append("/");
    m_PathName.append(pPath.native());
  }
  // a/,b or a,/b just append
  else {
    m_PathName.append(pPath.native());
  }
  return *this;
}

bool Path::empty() const
{
  return m_PathName.empty();
}

std::string Path::string() const
{
  return m_PathName;
}

Path::StringType Path::generic_string() const
{  
  return detail::canonical_form(*this);
}

void Path::canonicalize()
{  
  detail::canonical_form(*this);
}

Path::StringType::size_type Path::m_append_separator_if_needed()
{
  if (!m_PathName.empty() && is_directory(*this) &&
#ifdef LLVM_ON_WIN32
      *(m_PathName.end()-1) != colon && 
#endif
      !is_separator(*(m_PathName.end()-1))) {
        StringType::size_type tmp(m_PathName.size());
        m_PathName += preferred_separator;
        return tmp;
  }
  return 0;
}

void Path::m_erase_redundant_separator(Path::StringType::size_type pSepPos)
{
  size_t begin=pSepPos;
  // skip '/'
  while(separator == m_PathName[pSepPos])
    ++pSepPos;

  if(begin!=pSepPos)
    m_PathName.erase(begin+1,pSepPos-begin-1);
}

Path Path::spec_to_name() const
{
  std::string result;
  result.append("lib");
  result.append(m_PathName);
  result.append("lib");
  return result;
}

Path Path::stem() const
{
  std::string result;
  size_t found;
  found = m_PathName.find_last_of(separator);
  result = m_PathName.substr(found+1);
  found = result.find_first_of(".");
  result = result.substr(0,found-1);
  Path p(result);
  return p;
}

Path Path::extension() const
{
  std::string result;
  size_t found;
  found = m_PathName.find_last_of(separator);
  result = m_PathName.substr(found+1);
  found = result.find_first_of(".");
  result = m_PathName.substr(found+1);
  Path p(result);
  return p;
}

//===--------------------------------------------------------------------===//
// non-member functions
static bool mcld::sys::fs::exists(FileStatus f)
{
  return (f.type() != StatusError)&&(f.type() != FileNotFound);
}

static bool mcld::sys::fs::is_directory(FileStatus f)
{
  return f.type() == mcld::sys::fs::DirectoryFile;
}

bool mcld::sys::fs::operator==(const Path& pLHS,const Path& pRHS)
{
  return (pLHS.generic_string()==pRHS.generic_string());
}

bool mcld::sys::fs::operator!=(const Path& pLHS,const Path& pRHS)
{
  return !(pLHS==pRHS);
}

bool mcld::sys::fs::is_separator(char value)
{
  return (value == separator
#ifdef LLVM_ON_WIN32
          || value == preferred_separator
#endif
          );
}

bool mcld::sys::fs::exists(const Path &pPath)
{
  FileStatus pFileStatus;
  detail::status(pPath, pFileStatus);
  return exists(pFileStatus);
}

bool mcld::sys::fs::is_directory(const Path &pPath)
{
  FileStatus pFileStatus;
  detail::status(pPath, pFileStatus);
  return is_directory(pFileStatus);
}

std::ostream &mcld::sys::fs::operator<<(std::ostream& pOS,
                                        const Path& pPath)
{
  return pOS << pPath.native();
}

std::istream &mcld::sys::fs::operator>>(std::istream& pOS,
                                        const Path& pPath)
{
  return pOS >> pPath.native();
}

llvm::raw_ostream &mcld::sys::fs::operator<<(llvm::raw_ostream &pOS,
                                             const Path &pPath)
{
  return pOS << pPath.native();
}

