/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <Jush.Lu@mediatek.com>                                            *
 ****************************************************************************/
#ifndef MCOBJECTREADER_H
#define MCOBJECTREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/MC/MCLDFile.h"
#include "llvm/Support/system_error.h"

namespace mcld
{

//MCObjectReader provides an interface for different object formats.
class MCObjectReader
{
protected:
  MCObjectReader();
public:
  virtual ~MCObjectReader();
  virtual llvm::error_code readObject(const std::string &ObjectFile, 
                                      MCLDFile &) = 0;
};

} // namespace of mcld

#endif

