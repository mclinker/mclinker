//===- Linker.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LINKER_H
#define MCLD_LINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <string>

namespace mcld {

class Module;
class LinkerConfig;

class Target;
class TargetLDBackend;

class IRBuilder;
class ObjectLinker;

class FileHandle;
class MemoryArea;

/** \class Linker
*  \brief Linker is a modular linker.
*/
class Linker
{
public:
  Linker();

  ~Linker();

  /// config - To set up target-dependent options in pConfig.
  bool config(LinkerConfig& pConfig);

  /// resolve - To read participatory input files and build up mcld::Module
  bool resolve(Module& pModule, IRBuilder& pBuilder);

  /// layout - To serialize the final result of the output mcld::Module
  bool layout();

  /// link - A convenient way to resolve and to layout the output mcld::Module.
  bool link(Module& pModule, IRBuilder& pBuilder);

  /// emit - To emit output mcld::Module to a output MemoryArea
  bool emit(MemoryArea& pOutput);

  /// emit - To open a file for output in pPath and to emit output mcld::Module
  /// to the file.
  bool emit(const std::string& pPath);

  /// emit - To emit output mcld::Module in the pFileDescriptor.
  bool emit(int pFileDescriptor);

  bool reset();

private:
  bool initTarget();

  bool initBackend();

  bool initEmulator();

  bool initOStream();

private:
  LinkerConfig* m_pConfig;
  IRBuilder* m_pIRBuilder;

  const Target* m_pTarget;
  TargetLDBackend* m_pBackend;
  ObjectLinker* m_pObjLinker;
};

} // namespace of MC Linker

#endif

