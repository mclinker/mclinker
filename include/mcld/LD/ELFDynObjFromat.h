//===- header.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ELFDYNOBJFROMAT_H
#define ELFDYNOBJFROMAT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class ELFDynObjFromat
 *  \brief ELFDynObjFromat describes the format for ELF dynamic objects.
 */
class ELFDynObjFromat : public DynObjFormat
{

};

} // namespace of mcld

#endif

