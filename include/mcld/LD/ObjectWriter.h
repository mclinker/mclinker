//===- ObjectWriter.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LD_OBJECTWRITER_H
#define MCLD_LD_OBJECTWRITER_H
#include <system_error>

namespace mcld {

class Module;
class FileOutputBuffer;

/** \class ObjectWriter
 *  \brief ObjectWriter provides a common interface for object file writers.
 */
class ObjectWriter
{
protected:
  ObjectWriter();

public:
  virtual ~ObjectWriter();

  virtual std::error_code writeObject(Module& pModule,
                                      FileOutputBuffer& pOutput) = 0;

  virtual size_t getOutputSize(const Module& pModule) const = 0;
};

} // namespace of mcld

#endif

