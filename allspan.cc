//------------------------------------------------------------------------------
//
//  Spanning trees
//
//------------------------------------------------------------------------------

#include <iostream>

#include "graph.hpp"

using std::cout;
using std::endl;
using std::ofstream;

void outtabs(size_t n) {
  for (size_t i = 0; i < n; ++i)
    cout << "\t";
}

// this is recursive version of Mayeda-Seshu algorithm
// TODO: this can be done iterative
class all_spanning_MS {
  Graph &T;
  size_t N;
  set<size_t> D;
  vector<size_t> NUM;
  vector<bool> USED;

public:
  all_spanning_MS(Graph &G) : T(G), N(G.nvert()) {
    USED.resize(T.nrecords());
    D = spanning(T);
    NUM.push_back(-1); // NUM[0] means nothing
    T.forall_edges([&](size_t e) {
      NUM.push_back(e);
      USED[e] = true;
      return true;
    });
    // here NUM[1] .. NUM[N-1] filled
    assert(NUM.size() == N);
    for (auto e : D) {
      USED[e] = false;
      NUM.push_back(e);
    }
    // here NUM[N] .. NUM[M] filled
  }

  void do_all_spanning(function<void(Graph)> fcallback, size_t min,
                       size_t jmin) {
    fcallback(T);
    for (size_t eidx = min; eidx < N; ++eidx) {
      auto ei = NUM[eidx];
      T.edelete(ei);
      USED[ei] = false;
      for (size_t ejdx = jmin; ejdx < NUM.size(); ++ejdx) {
        auto ej = NUM[ejdx];
        if (USED[ej])
          continue;

        T.eundelete(ej);
        USED[ej] = true;
        if (detect_loop(T, T.vhead(ej) - 1).empty() &&
            is_connected(T, T.nvert()))
          do_all_spanning(fcallback, eidx + 1, ejdx + 1);
        T.edelete(ej);
        USED[ej] = false;
      }
      T.eundelete(ei);
      USED[ei] = true;
    }
  }

  void operator()(function<void(Graph)> fcallback) {
    do_all_spanning(fcallback, 1, N);
  }
};

// all spanning trees

template <typename F> int test_all() {
  size_t count;
  cout << "--- Test for all spanning trees by Mayeda-Seshu ---" << endl;
  count = 0;
  auto[ g, rep ] = get_rombic_graph(0);
  ofstream ofs("knuth_allspans_ms.dot");
  F spms(g);
  spms([&](Graph sp) {
    cout << "kgraph spanning #" << count++ << ": ";
    dump_flat(cout, sp);
    for (auto &x : rep) {
      x[1] += 2;
    }
    dump_as_dot(ofs, sp, rep);
  });

  count = 0;
  auto[ g23, rep23 ] = get_mn_lattice(2, 3);
  ofstream ofs23("lat23_allspans_ms.dot");
  F spms23(g23);
  spms23([&](Graph sp) {
    cout << "2-3 lattice spanning #" << count++ << ": ";
    dump_flat(cout, sp);
    for (auto &x : rep23) {
      x[1] += 3;
    }
    dump_as_dot(ofs23, sp, rep23);
  });
}

int main() { test_all<all_spanning_MS>(); }

