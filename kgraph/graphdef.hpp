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
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <utility>
#include <vector>

using std::array;
using std::begin;
using std::count;
using std::end;
using std::endl;
using std::find;
using std::function;
using std::make_pair;
using std::move;
using std::ostream;
using std::pair;
using std::remove_reference_t;
using std::vector;

namespace KGraph {

// print representation: coordinates for each vertex
using Rep = vector<array<size_t, 2>>;

static inline constexpr size_t even_one(size_t x) {
  return ((x % 2) == 0) ? x : (x^1); 
}

// mutable graph with runtime known number of vertices
class Graph final {

  // number of vertices
  size_t N;

  // edge records from 0 to N-1 has special meaning: these are tops of edge lists
  // edge pair (backward direction) always has number edge^1
  struct EdgeRecord {
    size_t vidx, next, prev;
  };

  vector<EdgeRecord> edges_;
  vector<size_t> degrees_;

// dependent types
public:
  using arr_t = vector<size_t>;
  using span_t = vector<size_t>;
  using arrit = typename arr_t::iterator;
  using marks_t = vector<bool>;

// dependent types initializers
public:
  arr_t init_arr() { vector<size_t> ret(N, 0); return ret; }
  span_t init_span() { vector<size_t> ret(N-1, 0); return ret; }
  marks_t init_marks() { vector<bool> ret(N, false); return ret; }
  size_t count_marks(marks_t &&m) { return count(m.begin(), m.end(), true); }

// construction
public:
  Graph(size_t);

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
  // This is not quite right for multigraphs
  bool equals(const Graph& rhs) const;

// helpers
private:
  // pseudo delete helper
  void edelete_impl(size_t edge);

  // pseudo undelete helper
  void eundelete_impl(size_t edge, size_t vidx);
};

//------------------------------------------------------------------------------
//
// Standalone operators
//
//------------------------------------------------------------------------------


static inline bool operator ==(const Graph &lhs, const Graph &rhs) {
  return lhs.equals(rhs);
}

static inline bool operator !=(const Graph &lhs, const Graph &rhs) {
  return !(lhs == rhs);
}

}

#endif
