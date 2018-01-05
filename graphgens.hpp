//------------------------------------------------------------------------------
//
//  Graph generators with coordinates
//
//------------------------------------------------------------------------------
//
// Graph is generated with print representation
// dot -Kfdp -n -Tpng knuth.dot > knuth.png
//
//------------------------------------------------------------------------------

#ifndef KNUTH_GRAPHGENS_GUARD_
#define KNUTH_GRAPHGENS_GUARD_

#include "graphdef.hpp"

namespace KGraph {

  // rombic graph is
  // 1-2, 1-3, then 1-by-N lattice like [2-3, 2-4, 3-5, 4-5], then 4-6 and 5-6
  // resulting graphs of order 0, 1 and 6 are: <|>, <||>, <|||||||>
  // Knuth used rombic order 0 as 7.2.1.6S algorithm example
  pair<Graph, Rep> get_rombic_graph(size_t);

  // like 2x3, 4x3, .... 
  pair<Graph, Rep> get_mn_lattice(size_t, size_t);
}

#endif
