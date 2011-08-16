/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_OUTPUT_H
#define MCLD_OUTPUT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/MC/MCLDFile.h>
#include <mcld/Support/RealPath.h>

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

