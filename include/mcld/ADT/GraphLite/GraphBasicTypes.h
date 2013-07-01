//===- GraphBasicTypes.h --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_GRAPHLITE_GRAPH_BASIC_TYPES_H
#define MCLD_GRAPHLITE_GRAPH_BASIC_TYPES_H

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

