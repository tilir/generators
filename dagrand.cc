#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <random>
#include <vector>

using std::atol;
using std::cout;
using std::default_random_engine;
using std::endl;
using std::ofstream;
using std::random_device;
using std::shuffle;
using std::uniform_int_distribution;
using std::vector;

void
gen_random(ofstream &ofs, int n, int emin, int emax) {
  assert (n > 3);
  assert (emin < emax);
  assert (emin > -1);

  int len = n * (n - 1) / 2; 

  random_device rd;
  default_random_engine gen {rd()};
  uniform_int_distribution<int> dist(emin, emax);

  int r = dist(gen);

  vector<bool> v(len);
  fill(v.end() - r, v.end(), true);
  shuffle(v.begin(), v.end(), gen);
    
  ofs << "digraph randag {" << endl;
  for (int i = 0; i < n; ++i) 
    ofs << "v" << i << ";" << endl;

  // ncount will be i*(i-1)/2 + j 
  // but easier way is just increment
  int ncount = 0;
 
  for (int i = 1; i < n; ++i) 
    for (int j = 0; j < i; ++j) {
      assert (ncount < len);
      if (v[ncount++]) 
        ofs << "v" << i << " -> " << "v" << j << ";" << endl; 
    }

  ofs << "}" << endl;
}

int
main (int argc, char **argv) {
  if ((argc < 2) || (argc > 4) || (atol(argv[1]) < 4)) {
    cout << "Usage: " << argv[0] << " n [emin] [emax]" << endl
         << "  will generate random.dot with uniform random dag with n vertices" << endl
         << "  please specify n > 3" << endl;
    return -1;
  }

  int n = atoi(argv[1]);

  int len = n * (n - 1) / 2; 

  int emin = len / 4, emax = 3 * len / 4;

  if (argc > 2)
    emin = atol(argv[2]);

  if (emin < 0) {
    cout << "emin parameter can not be < 0";
    return -1;
  }

  if (argc > 3)
    emax = atol(argv[3]);

  if (emax < emin) {
    cout << "emax parameter can not be < emin";
    return -1;
  }

  if (emax > len - 1) {
    cout << "emax parameter can not be > n * (n-1) / 2";
    return -1;
  }

  ofstream ofs("random.dot");
  gen_random(ofs, n, emin, emax);
}

