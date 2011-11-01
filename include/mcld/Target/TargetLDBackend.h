//===-- llvm/Target/TargetLDBackend.h - Target LD Backend -----*- C++ -*-===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_TARGET_TARGETLDBACKEND_H
#define LLVM_TARGET_TARGETLDBACKEND_H

namespace mcld {
class LDWriter;
class MCArchiveReader;
class MCObjectReader;
class MemoryArea;

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
  virtual LDWriter *getDSOWriter(MemoryArea *Area, bool _IsLittleEndian) = 0;
  virtual LDWriter *getEXEWriter(MemoryArea *Area, bool _IsLittleEndian) = 0;
  //virtual void readRelocation() = 0;
};

} // End mcld namespace

#endif
