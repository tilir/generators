//------------------------------------------------------------------------------
//
//  Graph representation for cache locality and easy operations
//
//------------------------------------------------------------------------------
//
// Idea is taken from Donald Knuth's Vol 4a
// Here undirected graph (V, E) represented as vector of edge records
// Record is { T, N, P }
// First V edge records correspond to vertices: T = 0, N is head of edges list
//                                              P is back-head of edges list
// if V+1 is odd, then one void record is inserted here to align
// Next E edge recoords represent edges. Each edge #E has corresponding 
// back edge #E^1. T is vertex number, N is next record on list, P is prev one
//
//------------------------------------------------------------------------------

#ifndef KNUTH_GRAPHDEF_GUARD_
#define KNUTH_GRAPHDEF_GUARD_

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>

using std::array;
using std::begin;
using std::bitset;
using std::cout;
using std::end;
using std::endl;
using std::forward;
using std::inserter;
using std::find;
using std::function;
using std::make_pair;
using std::move;
using std::ofstream;
using std::ostream;
using std::pair;
using std::remove_reference_t;
using std::set;
using std::stack;
using std::string;
using std::swap;
using std::vector;

namespace KGraph {

// print representation: coordinates for each vertex
template <size_t N>
using Rep = array<array<size_t, 2>, N>;

static inline constexpr size_t even_one(size_t x) {
  return ((x % 2) == 0) ? x : (x^1); 
}

// mutable graph with compile-time known number of vertices
template <size_t N>
class Graph final {
  // edge records from 0 to N-1 has special meaning: these are tops of edge lists
  // edge pair (backward direction) always has number edge^1
  struct EdgeRecord {
    size_t vidx, next, prev;
  };
  vector<EdgeRecord> edges_;
  array<size_t, N> degrees_ = {};

// array iterator
public:
  using arr_t = array<size_t, N>;
  using span_t = array<size_t, N-1>;
  using arrit = typename arr_t::iterator;
  using marks_t = bitset<N>;

// construction
public:
  Graph();

  // add undirected edge between start and fin as pair of edges
  // return pair of edges for start--fin and fin--start
  pair<size_t, size_t> add_edge(size_t start, size_t fin);

// simple getters
public:
  // number of vertices
  size_t nvert() const { return N; }

  // number of records
  size_t nrecords() const { return edges_.size(); }

  // degree of vertex
  size_t deg(size_t v) const { return degrees_[v - 1]; }

  // number of edges
  size_t nedges() const { return (edges_.size() - N) / 2; }

  // number of first edge
  size_t edges_start() const { return edges_.size() - (nedges() * 2); }

  // head vertex for edge
  size_t vhead(size_t e) const { return edges_[e].vidx; }

  // tail vertex for edge
  size_t vtail(size_t e) const { return edges_[e ^ 1].vidx; }

// dump representation
public:
  void dump(ostream&) const;

// forallx enumerators
public:
  // f(#vertex) returns true to continue process vertices
  // result is true if all are processed
  bool forall_vertices(function<bool(size_t)> f) const;

  // f(#edge) returns true to continue process edges
  // result is true if all are processed
  bool for_adjacent_edges(size_t idx, function<bool(size_t)> f) const;

  // forall edges once (i.e. for even records)
  // this can not be implemented as simple loop from start to end record
  // because there are edeleted ones
  bool forall_edges(function<bool(size_t)> f) const;

// delete and undelete edges
public:
  // pseudo delete for edge record (record itself kept unchanged)
  void edelete(size_t edge);

  // undelete for pseudo deleted edge record
  void eundelete(size_t edge);

// relations
public:
  // Equality means that all vertices have same sets of adjacent vertices
  // and same degrees. 
  // This is stronger, that isomorphism, but cheap
  // This is not quite right for multigraphs, so maybe bitset will become array
  bool equals(const Graph<N>& rhs) const;

// helpers
private:
  // pseudo delete helper
  void edelete_impl(size_t edge);

  // pseudo undelete helper
  void eundelete_impl(size_t edge, size_t vidx);
};

//------------------------------------------------------------------------------
//
// Graph methods
//
//------------------------------------------------------------------------------

template <size_t N>
Graph<N>::Graph(): edges_(N) {
  // filling special-meaning records 0 .. N-1
  for (size_t idx = 0; idx != N; ++idx) {
    edges_[idx] = {0, idx, idx};
    degrees_[idx] = 0;
  }

  // "aligning" vertex if total number is odd
  if constexpr ((N % 2) == 1)
    edges_.push_back({0, N, N});
}

template <size_t N>
pair<size_t, size_t> Graph<N>::add_edge(size_t start, size_t fin) {
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

template <size_t N>
void Graph<N>::dump(ostream &os) const {
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

template <size_t N>
bool Graph<N>::forall_vertices(function<bool(size_t)> f) const {
  for (size_t idx = 0; idx != N; ++idx) {
    bool res = f(idx);
    if (!res) return false;
  }
  return true;
}

template <size_t N>
bool Graph<N>::for_adjacent_edges(size_t idx, function<bool(size_t)> f) const {
  size_t edge = edges_[idx].next;
  while (edge > N - 1) {      
    bool res = f(edge);
    if (!res) return false;
    edge = edges_[edge].next;
  }
  return true;
}

template <size_t N>
bool Graph<N>::forall_edges(function<bool(size_t)> f) const {
  return forall_vertices([&] (size_t v) {
    return for_adjacent_edges(v, [&] (size_t e) {
      bool res = true;
      if (vhead(e) > vtail(e))
        res = f(e);          
      return res;
    });
  });
}

template <size_t N>
void Graph<N>::edelete(size_t edge) { 
  assert (edge < edges_.size());
  edge = even_one(edge);

  size_t start = vhead(edge);
  size_t fin = vtail(edge);

  degrees_[start - 1] -= 1;
  degrees_[fin - 1] -= 1;

  edelete_impl(edge);
  edelete_impl(edge ^ 1);
}

template <size_t N>
void Graph<N>::eundelete(size_t edge) { 
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

template <size_t N>
bool Graph<N>::equals(const Graph<N>& rhs) const {
  for (size_t v = 0; v < N; ++v) {
    bitset<N> adj;
    if (deg(v+1) != rhs.deg(v+1))
      return false;
    for_adjacent_edges(v, [&](size_t edge) {
      adj.set(vhead(edge) - 1);
      adj.set(vtail(edge) - 1);
      return true;
    });
    bool res = rhs.for_adjacent_edges(v, [&](size_t edge) {
      if (!adj.test(rhs.vhead(edge) - 1) || 
          !adj.test(rhs.vtail(edge) - 1))
        return false;
      return true;
    });
    if (!res)
     return false;
  }
  return true;
}

template <size_t N>
void Graph<N>::edelete_impl(size_t edge) { 
  auto oldprev = edges_[edge].prev;
  auto newnext = edges_[edge].next;
  edges_[oldprev].next = newnext; 
  edges_[newnext].prev = oldprev; 
}

template <size_t N>
void Graph<N>::eundelete_impl(size_t edge, size_t vidx) { 
  auto prev = edges_[vidx - 1].prev;
  auto next = edges_[prev].next;
  edges_[vidx - 1].prev = edge;
  edges_[prev].next = edge;

  edges_[edge].next = next;
  edges_[edge].prev = prev;
}


//------------------------------------------------------------------------------
//
// Standalone operators
//
//------------------------------------------------------------------------------


template <size_t N>
bool operator ==(const Graph<N> &lhs, const Graph<N> &rhs) {
  return lhs.equals(rhs);
}

template <size_t N>
bool operator !=(const Graph<N> &lhs, const Graph<N> &rhs) {
  return !(lhs == rhs);
}

}

#endif
