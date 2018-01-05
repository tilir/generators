
#include "graphgens.hpp"

namespace KGraph {

pair<Graph, Rep> get_rombic_graph(size_t N) {
  Graph g(N*2 + 4);
  Rep r(N*2 + 4);
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
pair<Graph, Rep> get_mn_lattice(size_t N, size_t M) {
  Graph g(M*N);
  assert(N > 0);
  assert(M > 1);
 
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
  Rep r(M*N);

  for (size_t xval = 0; xval != N; ++xval)
    for (size_t yval = 0; yval != M; ++yval)
      r[xval * M + yval] = {xval, yval};

  return make_pair(g, r);
}

}
