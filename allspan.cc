//------------------------------------------------------------------------------
//
//  Spanning trees
//
//------------------------------------------------------------------------------

#include <cstring>
#include <iostream>
#include <string>

#include "graph.hpp"

using std::cout;
using std::endl;
using std::ofstream;
using std::stol;
using std::strlen;

struct genconfig {
  bool only_stat = false;
  bool no_stat = false;
};

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

int all_span(size_t n, size_t m, genconfig gcf) {
  size_t count_st = 0;
  auto[ g, rep ] = get_mn_lattice(n, m);
  ofstream ofs("lat_allspans.dot");
  all_spanning_MS spms(g);
  spms([&](Graph sp) {
    count_st += 1;
    if (!gcf.only_stat) {
      cout << n << "-" << m << "lattice spanning #" << count_st << ": ";
      dump_flat(cout, sp);    
      for (auto &x : rep) {
        x[1] += m;
      }
      dump_as_dot(ofs, sp, rep);
    }
  });

  if (!gcf.no_stat) {
    cout << "Statistics:" << endl;
    cout << "Number of spanning trees: " << count_st << endl;
  }
}


void printusage(char *argv0) {
  cout << "Usage: " << argv0 << " n m [options]" << endl;
  cout << "\tWhere n is horizontal size" << endl;
  cout << "\t      m is vertical size" << endl;
  cout << "Note: m and n shall be >= 2" << endl;
  cout << "Options supported are:" << endl;
  cout << "\t-s -- show statistics only" << endl;
  cout << "\t-n -- show no statistics" << endl;
}

int main(int argc, char **argv) { 

  if (argc < 3) {
    printusage(argv[0]);
    return -1;
  }

  auto n = stol(argv[1]);
  auto m = stol(argv[2]);

  if (n < 2 || m < 2) {
    printusage(argv[0]);
    return -1;
  }

  genconfig gcf;

  for (size_t nopt = 3; nopt < argc; ++nopt) {
    if (argv[nopt][0] != '-') {
      printusage(argv[0]);
      cout << "Please prepend options with - and pass separately" << endl;
      return -1;
    }
    if (strlen(argv[nopt]) != 2) {
      printusage(argv[0]);
      cout << "Note: any option is one char after - sign" << endl;
      return -1;
    }
    switch (argv[nopt][1]) {
    case 's':
      gcf.only_stat = true;
      break;
    case 'n':
      gcf.no_stat = true;
      break;
    default:
      printusage(argv[0]);
      cout << "Note: only available options are listed above" << endl;
      return -1;
    }
  }

  all_span(n, m, gcf);
}

