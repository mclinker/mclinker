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
#include <mcld/MC/MCLDOutput.h>

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
class SectionMap;

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
  virtual bool initTargetSectionMap(SectionMap& pSectionMap) { return true;}
  virtual void initTargetSegments(MCLinker& pLinker) { }
  virtual void initTargetSections(MCLinker& pLinker) { }
  virtual void initTargetSymbols(MCLinker& pLinker) { }
  virtual void initTargetRelocation(MCLinker& pLinker) { }

  /// scanRelocation - determine the empty entries are needed or not for
  /// pReloc. If yes, create the empty entries.
  /// MCLinker traverse relocations to generate empty entries, such as
  /// GOT and dynamic relocation entries and other target dependent
  /// entries. These entries are generated for layout to adjust the ouput
  /// offset.
  /// @param pType - the MCLDOutput type of ouput file type
  virtual void scanRelocation(Relocation& pReloc,
                              MCLinker& pLinker,
                              unsigned int pType) { }

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

  /* The return value of machine() it the same as e_machine in the ELF header*/
  virtual uint32_t machine() const = 0;

  /** Is the target machine little endian? **/
  virtual bool isLittleEndian() const = 0;

  /** bit class. the bit length of the target machine, 32 or 64 **/
  virtual unsigned int bitclass() const = 0;

  /** section start offset in the output file **/
  virtual size_t sectionStartOffset() const = 0;
};

} // End mcld namespace

#endif
