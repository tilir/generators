//------------------------------------------------------------------------------
//
//  Spanning trees
//
//------------------------------------------------------------------------------

#include "graph.hpp"

void
outtabs(size_t n) {
  for (size_t i = 0; i < n; ++i)
    cout << "\t";
}

// this is recursive version of Mayeda-Seshu algorithm
// TODO: this can be done iterative
template <size_t N>
class all_spanning_MS {
  Graph<N>& T;
  set<size_t> D;
  vector<size_t> NUM;
  vector<bool> USED;

public:
  all_spanning_MS(Graph<N>& G) : T(G) {
    USED.resize(T.nrecords());
    D = spanning(T);
    NUM.push_back(-1); // NUM[0] means nothing
    T.forall_edges([&] (size_t e) {
      NUM.push_back(e);
      USED[e] = true;
      return true;
    });
    // here NUM[1] .. NUM[N-1] filled
    assert(NUM.size() == N);
    for (auto e: D) {
      USED[e] = false;
      NUM.push_back(e);
    }
    // here NUM[N] .. NUM[M] filled
  }    

  void do_all_spanning(function<void(Graph<N>)> fcallback, size_t min, size_t jmin) {
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
        if (detect_loop(T, T.vhead(ej) - 1).empty())
          do_all_spanning(fcallback, eidx+1, ejdx+1);
        T.edelete(ej);
        USED[ej] = false;
      }
      T.eundelete(ei);
      USED[ei] = true;
    }
  }

  void operator()(function<void(Graph<N>)> fcallback) {
    do_all_spanning(fcallback, 1, N);
  }
};

// all spanning trees

template<template <size_t> typename F>
int test_all() {
  size_t count;
  cout << "--- Test for all spanning trees by Mayeda-Seshu ---" << endl;
  count = 0;
  auto [g, rep] = get_rombic_graph<0>(); 
  ofstream ofs("knuth_allspans_ms.dot");
  F<4> spms(g);
  spms([&](Graph<4> sp) {
    cout << "kgraph spanning #" << count++ << ": ";
    dump_flat(cout, sp);
    for (auto &x : rep) { x[1] += 2; }
    dump_as_dot(ofs, sp, rep);
  });

  count = 0;
  auto [g23, rep23] = get_mn_lattice<2, 3>();
  ofstream ofs23("lat23_allspans_ms.dot");
  F<6> spms23(g23);
  spms23([&](Graph<6> sp) {
    cout << "2-3 lattice spanning #" << count++ << ": ";
    dump_flat(cout, sp);
    for (auto &x : rep23) { x[1] += 3; }
    dump_as_dot(ofs23, sp, rep23);
  });

  count = 0;
  auto [g33, rep33] = get_mn_lattice<3, 3>();
  ofstream ofs33("lat33_allspans_ms.dot");
  F<9> spms33(g33);
  spms33([&](Graph<9> sp) {
#if 0
    cout << "3-3 lattice spanning #" << count++ << ": ";
    dump_flat(cout, sp);
    for (auto &x : rep33) { x[1] += 3; }
    dump_as_dot(ofs33, sp, rep33);
#else
    count += 1;
#endif
  });
  cout << "Number of 3x3 lattices is " << count << endl;

  count = 0;
  auto [g43, rep43] = get_mn_lattice<4, 3>();
  ofstream ofs43("lat43_allspans_ms.dot");
  F<12> spms43(g43);
  spms43([&](Graph<12> sp) {
#if 0
    cout << "4-3 lattice spanning #" << count++ << ": ";
    dump_flat(cout, sp);
#else
    count += 1;
#endif
  });
  cout << "Number of 4x3 lattices is " << count << endl;
}

int
main() {
  test_all<all_spanning_MS>();
}
