//===- GraphBasicTypes.h --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ADT_GRAPHLITE_GRAPHBASICTYPES_H
#define MCLD_ADT_GRAPHLITE_GRAPHBASICTYPES_H

namespace mcld {
namespace graph {

/** \class UndirectedTag
 *  \brief Undirected graph category.
 */
struct UndirectedTag {};

/** \class DirectedTag
 *  \brief Directed graph category.
 */
struct DirectedTag {};

} // namespace of graph
} // namespace of mcld

#endif

