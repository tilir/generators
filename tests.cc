//------------------------------------------------------------------------------
//
//  Graph representation tests
//
//------------------------------------------------------------------------------

#include "graph.hpp"

using std::to_string;

template <typename T, typename R>
void do_dump(string name, T og, R rep) {
  ofstream ofs(name);
  dump_as_dot(ofs, og, rep);
}

int test_representation() {
  cout << "--- Test for representation ---" << endl;
  auto [g, rep] = get_rombic_graph<0>();

/*
  Dump shall give:

  a:  { 0,  1,  2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13}
  t:  { 0,  0,  0,   0,   1,   2,   1,   3,   2,   3,   2,   4,   3,   4}
  i:  {"", "", "",  "", "p", "p", "q", "q", "r", "r", "s", "s", "t", "t"}
  n:  { 5,  4,  6,  10,   9,   7,   8,   0,  13,  11,  12,   1,   3,   2}
  p:  { 7, 11,  13, 12,   1,   0,   2,   5,   6,   4,   3,   9,  10,   8}
*/

  g.dump(cout);
  do_dump("knuth.dot", g, rep);

  auto [g23, rep23] = get_mn_lattice<2, 3>();
  do_dump("lat23.dot", g23, rep23);

  auto [g33, rep33] = get_mn_lattice<3, 3>();
  do_dump("lat33.dot", g33, rep33);

  auto [g43, rep43] = get_mn_lattice<4, 3>();
  do_dump("lat43.dot", g43, rep43);

  cout << "Look at files: knuth.dot, lat23.dot, lat33.dot, lat43.dot" << endl;
  return 0;
}

// single spanning tree via DFS search
int test_dfs() {
  cout << "--- Test for single spanning tree via DFS ---" << endl;

  auto [g, rep] = get_rombic_graph<0>();
  auto os = spanning(g);
  do_dump("kspan.dot", g, rep);  

  auto [g23, rep23] = get_mn_lattice<2, 3>();
  auto os23 = spanning(g23);
  do_dump("lat23span.dot", g23, rep23);

  auto [g33, rep33] = get_mn_lattice<3, 3>();
  auto os33 = spanning(g33);
  do_dump("lat33span.dot", g33, rep33);

  auto [g43, rep43] = get_mn_lattice<4, 3>();
  auto os43 = spanning(g43);
  do_dump("lat43span.dot", g43, rep43);
  
  cout << "Look at files: kspan.dot, lat23span.dot, lat33span.dot, lat43span.dot" << endl;
  return 0;
}

// loop set finding
int test_loop_set() {
  cout << "--- Test for single loop set ---" << endl;

  auto [g, rep] = get_rombic_graph<0>();
  auto os = spanning(g);
  assert(!os.empty());
  g.eundelete(*os.begin());
  os.erase(os.begin());
  do_dump("kloop.dot", g, rep);
  cout << "Look at file: kloop.dot" << endl;
  auto cloop = detect_loop(g);
  for (auto e: cloop)
    cout << g.vhead(e) << "-" << g.vtail(e) << " ";
  cout << endl;  

  auto [g23, rep23] = get_mn_lattice<2, 3>();
  auto os23 = spanning(g23);
  assert(!os23.empty());
  g23.eundelete(*os23.begin());
  os23.erase(os23.begin());
  do_dump("lat23loop.dot", g23, rep23);
  cout << "Look at file: lat23loop.dot" << endl;
  auto cloop23 = detect_loop(g23);
  for (auto e: cloop23)
    cout << g23.vhead(e) << "-" << g23.vtail(e) << " ";  
  cout << endl;  

  auto [g33, rep33] = get_mn_lattice<3, 3>();
  auto os33 = spanning(g33);
  assert(!os33.empty());
  g33.eundelete(*os33.begin());
  os33.erase(os33.begin());
  do_dump("lat33loop.dot", g33, rep33);
  cout << "Look at file: lat33loop.dot" << endl;
  auto cloop33 = detect_loop(g33);
  for (auto e: cloop33)
    cout << g33.vhead(e) << "-" << g33.vtail(e) << " ";  
  cout << endl;  

  return 0;
}

int
test_equality() {
  cout << "--- Test for operator== ---" << endl;
  auto [g, rep] = get_rombic_graph<0>();
  auto edges = nonmod_spanning(g);
  for (auto e: edges) 
    cout << e << ": " << g.vhead(e) << "-" << g.vtail(e) << endl;
  Graph<4> newg;
  newg.add_edge(g.vhead(edges[0]), g.vtail(edges[0]));
  newg.add_edge(g.vhead(edges[1]), g.vtail(edges[1]));
  newg.add_edge(g.vhead(edges[2]), g.vtail(edges[2]));
  assert(newg != g);
  auto os = spanning(g);
  assert(!os.empty());
  assert(newg == g);
  newg.eundelete(*os.begin());
  assert(newg != g);
  return 0;
}

int
main () {
  test_representation();
  test_dfs();
  test_loop_set();
  test_equality();
}

