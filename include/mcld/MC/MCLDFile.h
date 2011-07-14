/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCLDFILE_H
#define MCLDFILE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/MC/MCLDContext.h>
#include <llvm/Support/MemoryBuffer.h>
#include <string>


namespace mcld
{
class MCLDContext;


class MCLDFile
{
public:
  enum attribute {
    GROUP_START,
    GROUP_END,
    NORMAL_FILE,
    UNKNOW
  };

  MCLDFile(/*FIXME*/ );
  ~MCLDFile();

   bool isRecognized() const;
  
private:
  std::string m_Filename;
  attribute m_Attribute;

  MCLDContext *m_pContext;
};

} // namespace of mcld

#endif

