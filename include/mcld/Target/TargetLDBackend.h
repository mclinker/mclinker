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

class Module;
class LinkerConfig;
class FragmentLinker;
class Relocation;
class RelocationFactory;
class Layout;
class ArchiveReader;
class ObjectReader;
class DynObjReader;
class ObjectWriter;
class DynObjWriter;
class ExecWriter;
class LDContext;
class LDFileFormat;
class LDSymbol;
class LDSection;
class SectionMap;
class Input;
class GOT;
class MemoryArea;
class MemoryAreaFactory;
class EhFrame;

//===----------------------------------------------------------------------===//
/// TargetLDBackend - Generic interface to target specific assembler backends.
///
class TargetLDBackend
{
  TargetLDBackend(const TargetLDBackend &);   // DO NOT IMPLEMENT
  void operator=(const TargetLDBackend &);  // DO NOT IMPLEMENT

protected:
  TargetLDBackend(const LinkerConfig& pConfig);

public:
  virtual ~TargetLDBackend();

  // -----  target dependent  ----- //
  virtual bool initTargetSectionMap(SectionMap& pSectionMap) { return true;}
  virtual void initTargetSegments(FragmentLinker& pLinker) { }
  virtual void initTargetSections(FragmentLinker& pLinker) { }
  virtual void initTargetSymbols(FragmentLinker& pLinker) { }
  virtual void initTargetRelocation(FragmentLinker& pLinker) { }
  virtual bool initStandardSymbols(FragmentLinker& pLinker) = 0;
  virtual bool initRelocFactory(const FragmentLinker& pLinker) = 0;

  virtual RelocationFactory* getRelocFactory() = 0;

  /// scanRelocation - When read in relocations, backend can do any modification
  /// to relocation and generate empty entries, such as GOT, dynamic relocation
  /// entries and other target dependent entries. These entries are generated
  /// for layout to adjust the ouput offset.
  /// @param pReloc - a read in relocation entry
  /// @param pInputSym - the input LDSymbol of relocation target symbol
  virtual void scanRelocation(Relocation& pReloc,
                              const LDSymbol& pInputSym,
                              FragmentLinker& pLinker,
                              const LDSection& pSection) = 0;

  // -----  format dependent  ----- //
  virtual ArchiveReader* createArchiveReader(Module&) = 0;
  virtual ObjectReader*  createObjectReader(FragmentLinker&) = 0;
  virtual DynObjReader*  createDynObjReader(FragmentLinker&) = 0;
  virtual ObjectWriter*  createObjectWriter(FragmentLinker&) = 0;
  virtual DynObjWriter*  createDynObjWriter(FragmentLinker&) = 0;
  virtual ExecWriter*    createExecWriter(FragmentLinker&) = 0;

  virtual bool initExecSections(FragmentLinker&) = 0;
  virtual bool initDynObjSections(FragmentLinker&) = 0;

  virtual LDFileFormat* getDynObjFileFormat() = 0;
  virtual LDFileFormat* getExecFileFormat() = 0;

  /// preLayout - Backend can do any needed modification before layout
  virtual void preLayout(FragmentLinker& pLinker) = 0;

  /// postLayout -Backend can do any needed modification after layout
  virtual void postLayout(Module& pModule, FragmentLinker& pLinker) = 0;

  /// postProcessing - Backend can do any needed modification in the final stage
  virtual void postProcessing(FragmentLinker& pLinker,
                              MemoryArea& pOutput) = 0;

  /// Is the target machine little endian? **/
  virtual bool isLittleEndian() const = 0;

  /// bit class. the bit length of the target machine, 32 or 64 **/
  virtual unsigned int bitclass() const = 0;

  /// the common page size of the target machine
  virtual uint64_t commonPageSize() const = 0;

  /// the abi page size of the target machine
  virtual uint64_t abiPageSize() const = 0;

  /// section start offset in the output file
  virtual size_t sectionStartOffset() const = 0;

  /// computeSectionOrder - compute the layout order of the given section
  virtual unsigned int getSectionOrder(const LDSection& pSectHdr) const = 0;

  /// sizeNamePools - compute the size of regular name pools
  /// In ELF executable files, regular name pools are .symtab, .strtab.,
  /// .dynsym, .dynstr, and .hash
  virtual void
  sizeNamePools(const Module& pModule) = 0;

  /// finalizeSymbol - Linker checks pSymbol.reserved() if it's not zero,
  /// then it will ask backend to finalize the symbol value.
  /// @return ture - if backend set the symbol value sucessfully
  /// @return false - if backend do not recognize the symbol
  virtual bool finalizeSymbols(FragmentLinker& pLinker) = 0;

  /// allocateCommonSymbols - allocate common symbols in the corresponding
  /// sections.
  virtual bool allocateCommonSymbols(Module& pModule, FragmentLinker& pLinker) const = 0;

  /// readSection - read a target dependent section
  virtual bool readSection(Input& pInput,
                           FragmentLinker& pLinker,
                           LDSection& pInputSectHdr)
  { return true; }

  /// dyld - the name of the default dynamic linker
  virtual const char* dyld() const = 0;

  /// sizeInterp - compute the size of program interpreter's name
  /// In ELF executables, this is the length of dynamic linker's path name
  virtual void sizeInterp() = 0;

public:
  EhFrame* getEhFrame();

  const EhFrame* getEhFrame() const;

protected:
  const LinkerConfig& config() const { return m_Config; }

private:
  /// m_pEhFrame - section .eh_frame
  EhFrame* m_pEhFrame;

  const LinkerConfig& m_Config;
};

} // End mcld namespace

#endif
