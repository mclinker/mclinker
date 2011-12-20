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
class LDContext;

class LDFileFormat;
class GOT;

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

  // -----  target dependent  ----- //
  virtual void initTargetSegments(MCLinker& pLinker) { }
  virtual void initTargetSections(MCLinker& pLinker) { }
  virtual void initTargetSymbols(MCLinker& pLinker) { }
  virtual void initTargetRelocation(MCLinker& pLinker) { }

  // -----  format dependent  ----- //
  virtual bool initArchiveReader(MCLinker&) = 0;
  virtual bool initObjectReader(MCLinker&) = 0;
  virtual bool initDynObjReader(MCLinker&) = 0;
  virtual bool initObjectWriter(MCLinker&) = 0;
  virtual bool initDynObjWriter(MCLinker&) = 0;

  virtual bool initExecSections(MCLinker&) = 0;
  virtual bool initDynObjSections(MCLinker&) = 0;

  virtual ArchiveReader *getArchiveReader() = 0;
  virtual ObjectReader *getObjectReader() = 0;
  virtual DynObjReader *getDynObjReader() = 0;
  virtual ObjectWriter *getObjectWriter() = 0;
  virtual DynObjWriter *getDynObjWriter() = 0;

  virtual LDFileFormat* getDynObjFileFormat() = 0;
  virtual LDFileFormat* getExecFileFormat() = 0;

  virtual RelocationFactory* getRelocFactory() = 0;

  /* The return value of machine() it the same as e_machine in the EFL header*/
  virtual uint32_t machine() const = 0;

  /** Is the target machine little endian? **/
  virtual bool isLittleEndian() const = 0;

  /** bit class. the bit length of the target machine, 32 or 64 **/
  virtual unsigned int bitclass() const = 0;

  /// getGOT - return the GOT
  /// Target can use co-variant return type to return its own GOT
  virtual GOT& getGOT() = 0;

  /// getGOT - return the GOT
  /// Target can use co-variant return type to return its own GOT
  virtual const GOT& getGOT() const = 0;
};

} // End mcld namespace

#endif
