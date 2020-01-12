//------------------------------------------------------------------------------
//
// Graph methods
//
//------------------------------------------------------------------------------

#include "graphdef.hpp"

namespace KGraph {

Graph::Graph(size_t nvert): N(nvert), edges_(nvert), degrees_(nvert) {
  // filling special-meaning records 0 .. N-1
  for (size_t idx = 0; idx != N; ++idx) {
    edges_[idx] = {0, idx, idx};
    degrees_[idx] = 0;
  }

  // "aligning" vertex if total number is odd
  if ((N % 2) == 1)
    edges_.push_back({0, N, N});
}

pair<size_t, size_t> Graph::add_edge(size_t start, size_t fin) {
  assert (start > 0);
  assert (fin > 0);
  assert (start != fin);
  size_t oldsz = edges_.size();
  assert ((oldsz % 2) == 0);
  size_t outedge = oldsz; // start ->
  size_t inedge = oldsz + 1; // -> end
  edges_.push_back({start, 0, 0});
  edges_.push_back({fin, 0, 0});
  eundelete(outedge);
  return make_pair(outedge, inedge);
}

void Graph::dump(ostream &os) const {
  os << "Graph of: " << N << " vertices and " << nedges() << " edges" << endl;  
  for (size_t cnt = 0; cnt < edges_.size(); ++cnt) 
    os << cnt << "\t";
  os << endl;
  for (auto e: edges_) 
    os << e.vidx << "\t";
  os << endl;
  for (auto e: edges_) 
    os << e.next << "\t";
  os << endl;
  for (auto e: edges_) 
    os << e.prev << "\t";
  os << endl;
}

bool Graph::forall_vertices(function<bool(size_t)> f) const {
  for (size_t idx = 0; idx != N; ++idx) {
    bool res = f(idx);
    if (!res) return false;
  }
  return true;
}

bool Graph::for_adjacent_edges(size_t idx, function<bool(size_t)> f) const {
  size_t edge = edges_[idx].next;
  while (edge > N - 1) {      
    bool res = f(edge);
    if (!res) return false;
    edge = edges_[edge].next;
  }
  return true;
}

bool Graph::forall_edges(function<bool(size_t)> f) const {
  return forall_vertices([&] (size_t v) {
    return for_adjacent_edges(v, [&] (size_t e) {
      bool res = true;
      if (vhead(e) > vtail(e))
        res = f(e);          
      return res;
    });
  });
}

void Graph::edelete(size_t edge) { 
  assert (edge < edges_.size());
  edge = even_one(edge);

  size_t start = vhead(edge);
  size_t fin = vtail(edge);

  degrees_[start - 1] -= 1;
  degrees_[fin - 1] -= 1;

  edelete_impl(edge);
  edelete_impl(edge ^ 1);
}

void Graph::eundelete(size_t edge) { 
  assert (edge < edges_.size());
  edge = even_one(edge);

  size_t inedge = edge^1;

  size_t start = vhead(edge);
  size_t fin = vtail(edge);

  degrees_[start - 1] += 1;
  degrees_[fin - 1] += 1;

  eundelete_impl(edge, start);
  eundelete_impl(inedge, fin);
}

bool Graph::equals(const Graph& rhs) const {
  for (size_t v = 0; v < N; ++v) {
    vector<bool> adj(N, false);
    if (deg(v+1) != rhs.deg(v+1))
      return false;
    for_adjacent_edges(v, [&](size_t edge) {
      adj[vhead(edge) - 1] = true;
      adj[vtail(edge) - 1] = true;
      return true;
    });
    bool res = rhs.for_adjacent_edges(v, [&](size_t edge) {
      if (!adj[rhs.vhead(edge) - 1] || 
          !adj[rhs.vtail(edge) - 1])
        return false;
      return true;
    });
    if (!res)
     return false;
  }
  return true;
}

void Graph::edelete_impl(size_t edge) { 
  auto oldprev = edges_[edge].prev;
  auto newnext = edges_[edge].next;
  edges_[oldprev].next = newnext; 
  edges_[newnext].prev = oldprev; 
}

void Graph::eundelete_impl(size_t edge, size_t vidx) { 
  auto prev = edges_[vidx - 1].prev;
  auto next = edges_[prev].next;
  edges_[vidx - 1].prev = edge;
  edges_[prev].next = edge;

  edges_[edge].next = next;
  edges_[edge].prev = prev;
}

}
