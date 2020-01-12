//------------------------------------------------------------------------------
//
//  Graph utilities, independet of graph representation
//
//------------------------------------------------------------------------------
//
// This utility collection is independent of exact graph representation
// to make it easier to switch between them
//
//------------------------------------------------------------------------------

#ifndef KNUTH_GRAPHUTIL_GUARD_
#define KNUTH_GRAPHUTIL_GUARD_

#include <set>
#include <utility>
#include <vector>

using std::forward;
using std::remove_reference_t;
using std::set;
using std::vector;

namespace KGraph {

//------------------------------------------------------------------------------
//
// interface
//
//------------------------------------------------------------------------------


// DFS-based methods (and DFS itself)

// determine back edges with dfs
// starts from start vertice (defaults to 0)
// hook returns true if we need to proceed search
//      implies signature (size_t edge, arrit start, arrit fin)
template <typename G, typename F>
void dfs_backedges(G&& graph, F fcb, size_t start = 0);

// makes in-place spanning tree by edeleting edges
// returns set of back edges
template <typename G>
set<size_t> spanning(G&& graph);

// non-modifying spanning tree
// returns set of N-1 edges which forms spanning tree
template <typename G>
auto nonmod_spanning(G&& graph);

// is connected component of size x
template <typename G>
bool is_connected(G&& graph, size_t x);

// returns set of looping edges
// starts from given vertex
template <typename G>
set<size_t> detect_loop(G&& graph, size_t start = 0);

// dumps one edge per line
template <typename G>
void dump_edges(ostream& ofs, G&& graph);

// dumps all edges in one line
template <typename G>
void dump_flat(ostream& ofs, G&& graph);

// dumps graph as dot
template <typename G, typename R>
void dump_as_dot(ostream& ofs, G&& graph, R pos = {});

// helper to calculate C = A - B
static inline set<size_t> disjoint (set<size_t> a, set<size_t> b);

//------------------------------------------------------------------------------
//
// implementation
//
//------------------------------------------------------------------------------

template <typename G, typename F>
void dfs_backedges(G&& graph, F fcb, size_t start) {
  typename remove_reference_t<G>::marks_t marks = graph.init_marks();
  typename remove_reference_t<G>::arr_t path = graph.init_arr();
  size_t curpos = 0;
  marks[start] = true;
  path[curpos++] = start;

  // path is stack of DFS-visited nodes. Do while it is not exhausted
  while (curpos > 0) {

    // current node is stack top
    auto curv = path[curpos - 1];

    // for current node find all unmarked successors
    bool not_found = graph.for_adjacent_edges(curv, [&](size_t cure) {
      assert (curv == graph.vhead(cure) - 1); 
      size_t targv = graph.vtail(cure) - 1;

      // immediate predecessor always marked
      if ((curpos > 1) && (path[curpos - 2] == targv))
        return true;

      // otherwise marked node means back edge if it already exists in path
      if (marks[targv]) {
        typename remove_reference_t<G>::arrit start = begin(path);
        typename remove_reference_t<G>::arrit fin = start + curpos - 1;
        if (find(start, fin, targv) != fin) {
          bool res = fcb(cure, start, fin);

          // caller via callback request termination: collapsing path to head
          if (!res) {
            curpos = 1;
            return false;
          }
        }

        // continue for_adjacent_edges
        return true;
      }

      // mark unmarked node and increment path
      marks[targv] = true;
      path[curpos++] = targv;
      return false;
    });

    // caller sayed stop to loop via callback: walking away

    // no unmarked successor found, means we need to decrement path    
    if (not_found)
      curpos -= 1;
  }  
}

template <typename G>
set<size_t> spanning(G&& graph) {
  set<size_t> edges;
  using arriter = typename remove_reference_t<G>::arrit;
  dfs_backedges(graph, [&](size_t cure, arriter, arriter){
    edges.insert(cure);
    graph.edelete(cure);
    return true;
  });
  return edges;
}

template <typename G>
auto nonmod_spanning(G&& graph) {
  vector v(graph.nrecords(), 1);
  using arriter = typename remove_reference_t<G>::arrit;
  dfs_backedges(forward<G>(graph), [&](size_t cure, arriter, arriter){
    v[cure] = 0;
    v[cure^1] = 0;
    return true;
  });

  size_t residx = 0;
  typename remove_reference_t<G>::span_t res = graph.init_span();
  for (size_t e = graph.edges_start(); e < graph.nrecords(); e += 2) {
    if (v[e] == 1)
      res[residx++] = e;
  }

  assert(residx == graph.nvert() - 1);
  return res;
}

template <typename G>
bool is_connected(G&& graph, size_t x) {
  typename remove_reference_t<G>::marks_t marks = graph.init_marks();
  graph.forall_edges([&](size_t e) {
    marks[graph.vhead(e) - 1] = true;
    marks[graph.vtail(e) - 1] = true;
    return true;
  });
  return graph.count_marks(move(marks)) == x;
}

template <typename G>
set<size_t> detect_loop(G&& graph, size_t start) {
  set<size_t> edges;
  using arriter = typename remove_reference_t<G>::arrit;
  dfs_backedges(graph, [&](size_t cure, arriter pstart, arriter pend){
    edges.insert(cure);
 
    // looking for actual loop start
    while (*pstart != graph.vtail(cure) - 1) {
      pstart++;
      assert (pstart != pend);
    }

    // now processing all path edges 
    for (auto it = pstart; it != pend - 1; ++it) {
      bool res = graph.for_adjacent_edges(*it, [&](size_t e) {
        assert(graph.vhead(e) - 1 == *it);
        if (graph.vtail(e) - 1 == *(it + 1)) {
          edges.insert(e);
          return false;
        }
        return true;        
      });
      assert (res == false);
    }

    // final edge from last path vertex to loop edge head
    bool res = graph.for_adjacent_edges(*(pend - 1), [&](size_t e) {
      if (graph.vtail(e) - 1 == graph.vhead(cure) - 1) {
        edges.insert(e);
        return false;
      }
      return true;
    });
    assert (res == false);
    return false;
  }, start);
  return edges;
}


template <typename G>
void dump_edges(ostream& ofs, G&& graph) {
  graph.forall_edges([&] (size_t e) {
    ofs << "v" << graph.vhead(e) << " -- v" << graph.vtail(e) << endl; 
    return true;
  });
}

template <typename G>
void dump_flat(ostream& ofs, G&& graph) {
  graph.forall_edges([&] (size_t e) {
    ofs << "v" << graph.vhead(e) << " -- v" << graph.vtail(e) << " "; 
    return true;
  });
  ofs << endl;
}

template <typename G, typename R>
void dump_as_dot(ostream& ofs, G&& graph, R pos) {
  ofs << "strict graph {" << endl;
  graph.forall_vertices([&] (size_t idx) {
    ofs << "v" << idx + 1;
    if (pos.size() > idx) {
      ofs << "[pos = \"" << pos[idx][0] << "," << pos[idx][1] << "!\"]";
    }
    ofs << ";" << endl;
    return true;
  });
 
  dump_edges(ofs, forward<G>(graph));
    
  ofs << "}" << endl;
}

static inline set<size_t> disjoint (set<size_t> a, set<size_t> b) {
  set<size_t> c;
  set_difference(a.begin(), a.end(), 
                 b.begin(), b.end(), 
                 inserter(c, c.begin()));
  return c;
}

}

#endif
