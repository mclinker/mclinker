//===- Uncopyable.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ADT_UNCOPYABLE_H_
#define MCLD_ADT_UNCOPYABLE_H_

namespace mcld
{

/** \class Uncopyable
 *  \brief Uncopyable provides the base class to forbit copy operations.
 *
 */
class Uncopyable
{
protected:
  Uncopyable() { }
  ~Uncopyable() { }

private:
  Uncopyable(const Uncopyable&); /// NOT TO IMPLEMENT
  Uncopyable& operator=(const Uncopyable&); /// NOT TO IMPLEMENT
};

} // namespace mcld

#endif  // MCLD_ADT_UNCOPYABLE_H_
