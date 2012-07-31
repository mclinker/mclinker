//===- Path.cpp -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Support/FileSystem.h"
#include "mcld/Support/Path.h"
#include <llvm/ADT/StringRef.h>

#include <locale>
#include <string.h>
#include <istream>
#include <ostream>

using namespace mcld;
using namespace mcld::sys::fs;

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

Path::Path(const Path& pCopy)
 : m_PathName(pCopy.m_PathName) {
}

Path::~Path()
{
}

bool Path::isFromRoot() const
{
  if (m_PathName.empty())
    return false;
  return (separator == m_PathName[0]);
}

bool Path::isFromPWD() const
{
  if (2 > m_PathName.size())
    return false;
  return ('.' == m_PathName[0] && separator == m_PathName[1]);
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
  std::string result = m_PathName;
  detail::canonicalize(result);
  return result;
}

bool Path::canonicalize()
{
  return detail::canonicalize(m_PathName);
}

Path::StringType::size_type Path::m_append_separator_if_needed()
{
  if (!m_PathName.empty() &&
#if defined(MCLD_ON_WIN32)
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

Path Path::parent_path() const
{
  size_t end_pos = m_PathName.find_last_of(separator);
  if (end_pos != StringType::npos)
    return Path(m_PathName.substr(0, end_pos));
  return Path();
}

Path Path::stem() const
{
  size_t begin_pos = m_PathName.find_last_of(separator)+1;
  size_t end_pos   = m_PathName.find_last_of(".");
  Path result_path(m_PathName.substr(begin_pos, end_pos - begin_pos));
  return result_path;
}

Path Path::extension() const
{
  size_t begin_pos = m_PathName.find_last_of('.');
  Path result_path(m_PathName.substr(begin_pos));
  return result_path;
}

//===--------------------------------------------------------------------===//
// non-member functions
//===--------------------------------------------------------------------===//
bool mcld::sys::fs::operator==(const Path& pLHS,const Path& pRHS)
{
  return (pLHS.generic_string()==pRHS.generic_string());
}

bool mcld::sys::fs::operator!=(const Path& pLHS,const Path& pRHS)
{
  return !(pLHS==pRHS);
}

Path mcld::sys::fs::operator+(const Path& pLHS, const Path& pRHS)
{
  mcld::sys::fs::Path result = pLHS;
  result.append(pRHS);
  return result;
}

bool mcld::sys::fs::is_separator(char value)
{
  return (value == separator
#if defined(MCLD_ON_WIN32)
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
                                        Path& pPath)
{
  return pOS >> pPath.native();
}

llvm::raw_ostream &mcld::sys::fs::operator<<(llvm::raw_ostream &pOS,
                                             const Path &pPath)
{
  return pOS << pPath.native();
}

