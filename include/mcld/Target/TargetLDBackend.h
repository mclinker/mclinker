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

class MCLinker;
class MCELFLDTargetWriter;
class MCLDWriter;
class MCArchiveReader;
class MCObjectReader;
class MCObjectWriter;
class Relocation;
class RelocationFactory;
class Layout;

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
  virtual void addTargetSegments(MCLinker& pLinker) { }
  virtual void addTargetSections(MCLinker& pLinker) { }
  virtual void addTargetSymbols(MCLinker& pLinker) { }
  virtual void addTargetRelocation(MCLinker& pLinker) { }

  virtual MCArchiveReader *getArchiveReader() = 0;
  virtual MCObjectReader *getObjectReader() = 0;
  virtual MCObjectWriter *getObjectWriter() = 0;
  virtual RelocationFactory* getRelocFactory() = 0;

};

} // End mcld namespace

#endif
