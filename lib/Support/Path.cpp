/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Support/Path.h>
#include <llvm/ADT/StringRef.h>
#include <locale>

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

bool Path::empty() const
{
  return m_PathName.empty();
}

std::string Path::string() const
{
  return m_PathName;
}

Path::StringType::size_type Path::m_append_separator_if_needed()
{
  if (!m_PathName.empty() &&
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

void Path::m_erase_redundant_separator(StringType &pPathName,
                                       Path::StringType::size_type pSepPos) const
{
  if (pSepPos                         // a separator was added
      && pSepPos < pPathName.size()         // and something was appended
      && (pPathName[pSepPos+1] == separator // and it was also separator
#ifdef LLVM_ON_WIN32
       || pPathName[pSepPos+1] == preferred_separator  // or preferred_separator
#endif
      )) {
    pPathName.erase(pSepPos, 1);
  } // erase the added separator
}


//===--------------------------------------------------------------------===//
// non-member functions
inline bool mcld::sys::fs::is_separator(char value)
{
  return (value == separator
#ifdef LLVM_ON_WIN32
          || value == preferred_separator
#endif
          );
}

bool mcld::sys::fs::exists(const Path &pPath)
{
  // csmon! I'm here
}

bool mcld::sys::fs::is_directory(const Path &pPath)
{
  // csmon! I'm here
}

std::ostream &mcld::sys::fs::operator<<(std::ostream& pOS,
                                        const Path& pPath)
{
  return pOS << pPath.native();
}

std::istream &mcld::sys::fs::operator>>(std::istream& pOS,
                                        const Path& pPath)
{
  // FIXME
}

llvm::raw_ostream &mcld::sys::fs::operator<<(llvm::raw_ostream &pOS,
                                             const Path &pPath)
{
  return pOS << pPath.native();
}

// Include the truly platform-specific parts.
#if defined(LLVM_ON_UNIX)
#include "Unix/PathV3.inc"
#endif
#if defined(LLVM_ON_WIN32)
#include "Windows/PathV3.inc"
#endif

