//===-- llvm/Target/TargetLDBackend.h - Target LD Backend -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_TARGET_TARGETLDBACKEND_H
#define LLVM_TARGET_TARGETLDBACKEND_H

namespace mcld {
class MCELFLDTargetWriter;
class MCLDWriter;
class MCArchiveReader;
class MCObjectReader;
class MCObjectWriter;

//===----------------------------------------------------------------------===//
/// TargetLDBackend - Generic interface to target specific assembler backends.
///
class TargetLDBackend
{
  TargetLDBackend(const TargetLDBackend &);   // DO NOT IMPLEMENT
  void operator=(const TargetLDBackend &);  // DO NOT IMPLEMENT

protected:
  TargetLDBackend();

public:
  virtual ~TargetLDBackend();

  virtual MCArchiveReader *getArchiveReader() = 0;
  virtual MCObjectReader *getObjectReader() = 0;
  virtual MCObjectWriter *getObjectWriter() = 0;

  //virtual void readRelocation() = 0;
};

} // End mcld namespace

#endif
