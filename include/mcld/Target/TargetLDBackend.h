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
namespace llvm {
class MCObjectWriter;
} //namespace of LLVM

namespace mcld {
class MCELFLDTargetWriter;
class MCLDWriter;
class TargetArchiveReader;
class TargetObjectReader;

//===----------------------------------------------------------------------===//
/// TargetLDBackend - Generic interface to target specific assembler backends.
///
class TargetLDBackend 
{
  TargetLDBackend(const TargetLDBackend &);   // DO NOT IMPLEMENT
  void operator=(const TargetLDBackend &);  // DO NOT IMPLEMENT

public:
  typedef TargetArchiveReader *(*TargetArchiveReaderCtorFnTy)(void);
  typedef TargetObjectReader *(*TargetObjectReaderCtorFnTy)(void);

protected:
  TargetArchiveReaderCtorFnTy TargetArchiveReaderCtorFn;
  TargetObjectReaderCtorFnTy  TargetObjectReaderCtorFn;

protected: // Can only create subclasses.
  TargetLDBackend(TargetArchiveReaderCtorFnTy pTACF, TargetObjectReaderCtorFnTy pTOCF);

public:
  virtual ~TargetLDBackend();
};

} // End mcld namespace

#endif
