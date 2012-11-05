//===- IRBuilder.h --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// IRBuilder is a class used as a convenient way to create MCLinker sections
// with a consistent and simplified interface.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_IRBUILDER_H
#define MCLD_IRBUILDER_H

#include <mcld/MC/MCLDInput.h>

#include <mcld/Support/Path.h>

namespace mcld {

class Module;
class LinkerConfig;
class InputTree;

/** \class IRBuilder
 *  \brief IRBuilder provides an uniform API for creating sections and
 *  inserting them into a input file.
 */
class IRBuilder
{
public:
  IRBuilder(Module& pModule, InputTree& pInputs, const LinkerConfig& pConfig);

  ~IRBuilder();

/// @}
/// @name Input Files On The Command Line
/// @{

  /// CreateInput - Make a new input file and append it on the input tree.
  ///
  /// Calling this function is equal to add the path of an input file in
  /// the command line of MCLinker.
  ///
  /// There are four types of the input files:
  ///   - relocatable objects,
  ///   - shared objects,
  ///   - archives,
  ///   - and user-defined objects.
  ///
  /// If the given type is Input::Unknown, MCLinker will automatically
  /// recognize the type of the input file and create sections of the input. 
  /// If the given type is not Input::Unknown, user must create sections and
  /// fragments manunally.
  ///
  /// @see mcld::Input
  /// 
  /// @param pPath [in] The path of the input file.
  /// @param pType [in] The type of the input file. MCLinker will parse the
  ///                   input file to create sections only if pType is
  ///                   Input::Unknown.
  /// @return the created mcld::Input. The name of the created input is set as
  /// the filename of the pPath.
  Input* CreateInput(const sys::fs::Path& pPath,
                     unsigned int pType = Input::Unknown);

  /// CreateInput - Make a new input file and append it on the input tree.
  ///
  /// @param pName [in] The name of the input file.
  /// @param pPath [in] The path of the input file.
  /// @param pType [in] The type of the input file. MCLinker will parse the
  ///                   input file to create sections only if pType is
  ///                   Input::Unknown.
  /// @return the created mcld::Input.
  Input* CreateInput(const std::string& pName,
                     const sys::fs::Path& pPath,
                     unsigned int pType = Input::Unknown);

  /// CreateInput - Make a new input file and appended it on the input tree.
  ///
  /// This function is equal to -l option. This function tells MCLinker to
  /// search for lib[pNameSpec].so or lib[pNameSpec].a in the search path.
  ///
  /// @param pNameSpec [in] The namespec of the input file.
  /// @return the created mcld::Input.
  Input* CreateInput(const std::string& pNameSpec);

  /// StartGroup - Add an opening tag of group.
  ///
  /// This function is equal to --start-group option. This function tells
  /// MCLinker to create a new archive group and to add the following archives
  /// in the created group. The archives in a group are searched repeatedly
  /// until no new undefined references are created.
  void StartGroup();

  /// EndGroup - Add a closing tag of group.
  ///
  /// This function is equal to --end-group option. This function tells
  /// MCLinker to stop adding following archives in the created group.
  void EndGroup();

/// @}
/// @name Positional Options On The Command Line
/// @{

  /// WholeArchive - Append a --whole-archive option on the command line
  ///
  /// This function is equal to --whole-archive option. This function tells
  /// MCLinker to include every object files in the following archives.
  void WholeArchive();

  /// NoWholeArchive - Append a --no-whole-archive option on the command line.
  ///
  /// This function is equal to --no-whole-archive option. This function tells
  /// MCLinker to stop including every object files in the following archives.
  /// Only used object files in the following archives are included.
  void NoWholeArchive();

  /// AsNeeded - Append a --as-needed option on the command line.
  ///
  /// This function is equal to --as-needed option. This function tells
  /// MCLinker to not add a DT_NEEDED tag in .dynamic sections for the
  /// following shared objects that are not really used. MCLinker will add tags
  //  only for the following shared objects which is really used.
  void AsNeeded();

  /// NoAsNeeded - Append a --no-as-needed option on the command line.
  ///
  /// This function is equal to --no-as-needed option. This function tells
  /// MCLinker to add a DT_NEEDED tag in .dynamic section for every shared
  /// objects that is created after this option.
  void NoAsNeeded();

  /// CopyDTNeeded - Append a --add-needed option on the command line.
  ///
  /// This function is equal to --add-needed option. This function tells
  /// NCLinker to copy all DT_NEEDED tags of every following shared objects
  /// to the output file.
  void CopyDTNeeded();

  /// NoCopyDTNeeded - Append a --no-add-needed option on the command line.
  ///
  /// This function is equal to --no-add-needed option. This function tells
  /// MCLinker to stop copying all DT_NEEDS tags in the following shared
  /// objects to the output file.
  void NoCopyDTNeeded();

  /// AgainstShared - Append a -Bdynamic option on the command line.
  ///
  /// This function is equal to -Bdynamic option. This function tells MCLinker
  /// to search shared objects before archives for the following namespec.
  void AgainstShared();

  /// AgainstStatic - Append a -static option on the command line.
  ///
  /// This function is equal to -static option. This function tells MCLinker to
  /// search archives before shared objects for the following namespec.
  void AgainstStatic();

/// @}
/// @name Input Methods
/// @{

  /// CreateHeader - To create and append a section in the input file.
  ///
  /// @param pInput [in, out] The input file.
  /// @param pName  [in]      The name of the section.
  /// @param pKind  [in]      The kind of the section. Section kind affects the
  ///                         way to merge this section.
  /// @param pType  [in]      The meaning of the content in the section. The
  ///                         value is format-dependent. In ELF, the value is
  ///                         SHT_* in normal.
  /// @param pFlag  [in]      The format-dependent flag. In ELF, the value is
  ///                         SHF_* in normal.
  /// @param pAlign [in]      The alignment constraint of the section
  /// @return The created section header.
  LDSection* CreateHeader(Input& pInput,
                          const std::string& pName,
                          LDFileFormat::Kind pKind,
                          uint32_t pType,
                          uint32_t pFlag,
                          uint32_t pAlign);

  /// AppendFragment - To append a fragment in the section.
  /// To append pFrag and to increase the size of appended section.
  ///
  /// @param [in, out] pFrag    The appended fragment. The offset of the
  ///   fragment is also be adjusted.
  /// @param [in, out] pSection The section being appended. The size of the
  ///   section is adjusted. The offset of the other sections in the same input
  ///   file are also be adjusted.
  /// @param [out]     pInput   The input file which contains the section.
  bool AppendFragment(Fragment& pFrag, LDSection& pSection, Input& pInput);

private:
  Module& m_Module;
  InputTree& pInputTree;
  const LinkerConfig& pConfig;
};

} // end of namespace mcld

#endif
