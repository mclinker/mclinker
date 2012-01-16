//===- MCLDOutput.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  Output class inherits MCLDFile, which is used to represent a output file.
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_OUTPUT_H
#define MCLD_OUTPUT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/MC/MCLDFile.h>
#include <mcld/Support/RealPath.h>
#include <string>

namespace mcld
{

/** \class MCLDOutput
 *  \brief MCLDOutput provides the information about the output.
 *
 *  @see MCLDFile
 */
class Output : public MCLDFile
{
public:
  enum Type {
    Object = MCLDFile::Object,
    DynObj = MCLDFile::DynObj,
    Exec   = MCLDFile::Exec
  };

public:
  Output();
  explicit Output(const sys::fs::Path& pRealPath,
                  Type pType);

  ~Output();
};

} // namespace of mcld

#endif

