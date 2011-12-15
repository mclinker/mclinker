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

#include <llvm/Support/DataTypes.h>

namespace mcld {

class MCLinker;
class Relocation;
class RelocationFactory;
class Layout;
class ArchiveReader;
class ObjectReader;
class DynObjReader;
class ObjectWriter;
class DynObjWriter;

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

  virtual bool initArchiveReader(MCLinker&) = 0;
  virtual bool initObjectReader(MCLinker&) = 0;
  virtual bool initDynObjReader(MCLinker&) = 0;
  virtual bool initObjectWriter(MCLinker&) = 0;
  virtual bool initDynObjWriter(MCLinker&) = 0;

  virtual ArchiveReader *getArchiveReader() = 0;
  virtual ObjectReader *getObjectReader() = 0;
  virtual DynObjReader *getDynObjReader() = 0;
  virtual ObjectWriter *getObjectWriter() = 0;
  virtual DynObjWriter *getDynObjWriter() = 0;

  virtual RelocationFactory* getRelocFactory() = 0;

  /* The return value of machine() it the same as e_machine in the EFL header*/
  virtual uint32_t machine() const = 0;

};

} // End mcld namespace

#endif
