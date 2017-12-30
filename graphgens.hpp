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
// knuth used rombic order 0 as 7.2.1.6S algorithm example
template<size_t N>
pair<Graph<N*2 + 4>, Rep<N*2 + 4>> get_rombic_graph() {
  Graph<N*2 + 4> g;   
  Rep<N*2 + 4> r;
  g.add_edge(1, 2);
  r[0] = {0, 1};
  g.add_edge(1, 3);
  r[1] = {1, 0};
  g.add_edge(2, 3);
  r[2] = {1, 2};
  size_t higher = 2;
  size_t lower = 3;

  for (size_t x = 0; x < N; ++x) {
    r[lower] = {higher/2 + 1, 0};
    r[lower + 1] = {higher/2 + 1, 2};
    g.add_edge(higher, higher+2);
    g.add_edge(lower, lower+2);
    higher += 2;
    lower += 2;
    g.add_edge(higher, lower);
  }

  assert (lower + 1 == N*2 + 4);

  r[lower] = {higher/2 + 1, 1};
  g.add_edge(higher, lower+1);
  g.add_edge(lower, lower+1);
  return make_pair(g, r);
}

// like 2x3, 4x3, ....
template<size_t N, size_t M>
pair<Graph<M*N>, Rep<M*N>> get_mn_lattice() {
  Graph<M*N> g;
  static_assert(N > 0);
  static_assert(M > 1);
 
  // first layer like: 1-2, 2-3
  for (size_t yval = 1; yval != M; ++yval) 
    g.add_edge(yval, yval + 1);   

  // other layers like: 4-5, 1-4, 5-6, 2-5, 3-6
  for (size_t xval = 1; xval != N; ++xval) {
    for (size_t yval = 1; yval != M; ++yval) {
      g.add_edge(yval + xval*M, yval + xval*M + 1);
      g.add_edge(yval + (xval-1)*M, yval + xval*M); 
    }
    g.add_edge(xval*M, (xval + 1)*M);
  }

  // rep like (0, 0), (0, 1), (0, 2), (1, 0) ...
  Rep<M*N> r;

  for (size_t xval = 0; xval != N; ++xval)
    for (size_t yval = 0; yval != M; ++yval)
      r[xval * M + yval] = {xval, yval};

  return make_pair(g, r);
}

}

#endif