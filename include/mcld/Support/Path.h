//===- Path.h -------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This file declares the mcld::sys::fs:: namespace. It follows TR2/boost
// filesystem (v3), but modified to remove exception handling and the
// path class.
//===----------------------------------------------------------------------===//
#ifndef MCLD_PATH_H
#define MCLD_PATH_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/Support/raw_ostream.h>
#include <mcld/Config/Config.h>

#include <iosfwd>
#include <functional>
#include <string>

namespace mcld {
namespace sys  {
namespace fs   {

#if defined(MCLD_ON_WIN32)
const wchar_t       separator = L'\\';
const wchar_t       preferred_separator = L'\\';
#else
const char          separator = '/';
const char          preferred_separator = '/';
#endif

/** \class Path
 *  \brief Path provides an abstraction for the path to a file or directory in
 *   the operating system's filesystem.
 *
 *  FIXME: current Path library only support UTF-8 chararcter set.
 *
 */
class Path
{
public:
#if defined(MCLD_ON_WIN32)
  typedef wchar_t                            ValueType;
#else
  typedef char                               ValueType;
#endif
  typedef std::basic_string<ValueType>       StringType;

public:
  Path();
  Path(const ValueType* s);
  Path(const StringType &s);
  Path(const Path& pCopy);
  virtual ~Path();

  // -----  assignments  ----- //
  template <class InputIterator>
  Path& assign(InputIterator begin, InputIterator end);
  Path& assign(const StringType &s);
  Path& assign(const ValueType* s, unsigned int length);

  //  -----  appends  ----- //
  template <class InputIterator>
  Path& append(InputIterator begin, InputIterator end);
  Path& append(const Path& pPath);

  //  -----  observers  ----- //
  bool empty() const;

  bool isFromRoot() const;
  bool isFromPWD() const;

  const StringType &native() const
  { return m_PathName; }

  StringType &native()
  { return m_PathName; }

  const ValueType* c_str() const
  { return m_PathName.c_str(); }

  std::string string() const;

  // -----  decomposition  ----- //
  Path parent_path() const;
  Path stem() const;
  Path extension() const;

  // -----  generic form observers  ----- //
  StringType generic_string() const;
  bool canonicalize();

public:
  StringType::size_type m_append_separator_if_needed();
  void m_erase_redundant_separator(StringType::size_type sep_pos);

protected:
  StringType m_PathName;
};

bool operator==(const Path& pLHS,const Path& pRHS);
bool operator!=(const Path& pLHS,const Path& pRHS);
Path operator+(const Path& pLHS, const Path& pRHS);

//--------------------------------------------------------------------------//
//                              non-member functions                        //
//--------------------------------------------------------------------------//

/// is_separator - is the given character a separator of a path.
// @param value a character
// @result true if \a value is a path separator character on the host OS
//bool status_known(FileStatus f) { return f.type() != StatusError; }

bool is_separator(char value);

bool exists(const Path &pPath);

bool is_directory(const Path &pPath);


std::ostream &operator<<(std::ostream& pOS, const Path& pPath);

std::istream &operator>>(std::istream& pOS, Path& pPath);

llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const Path &pPath);


//--------------------------------------------------------------------------------------//
//                     class path member template implementation                        //
//--------------------------------------------------------------------------------------//
template <class InputIterator>
Path& Path::assign(InputIterator begin, InputIterator end)
{
  m_PathName.clear();
  if (begin != end)
    m_PathName.append<InputIterator>(begin, end);
  return *this;
}

template <class InputIterator>
Path& Path::append(InputIterator begin, InputIterator end)
{
  if (begin == end)
    return *this;
  StringType::size_type sep_pos(m_append_separator_if_needed());
  m_PathName.append<InputIterator>(begin, end);
  if (sep_pos)
    m_erase_redundant_separator(sep_pos);
  return *this;
}

} // namespace of fs
} // namespace of sys
} // namespace of mcld

//-------------------------------------------------------------------------//
//                              STL compatible functions                   //
//-------------------------------------------------------------------------//
namespace std {

template<>
struct less<mcld::sys::fs::Path> : public binary_function<mcld::sys::fs::Path,
                                                         mcld::sys::fs::Path,
                                                         bool>
{
  bool operator() (const mcld::sys::fs::Path& pX,const mcld::sys::fs::Path& pY) const {
    if (pX.generic_string().size() < pY.generic_string().size())
      return true;
    return (pX.generic_string() < pY.generic_string());
  }
};

} // namespace of std

#endif

