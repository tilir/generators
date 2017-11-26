#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::next_permutation;
using std::ofstream;
using std::string;
using std::to_string;
using std::vector;

int
gen_all(string basic_name, unsigned n) {
  assert (n > 1);
  unsigned len = n * (n - 1) / 2;  

  for (unsigned r = 1; r < len; ++r) {
    string name = basic_name + to_string(n) + string(".")  + to_string(r) + string(".dot"); 
    ofstream ofs(name);
    vector<bool> v(len);
    fill(v.end() - r, v.end(), true);
    
    unsigned pcount = 0;
    do {
      ofs << "digraph " << r << pcount << " {" << endl;
      for (unsigned i = 0; i < n; ++i) 
        ofs << "v" << i << ";" << endl;
      unsigned ncount = 0;
      for (unsigned i = 1; i < n; ++i) 
        for (unsigned j = 0; j < i; ++j) {
          assert (ncount < len);
          if (v[ncount++]) 
            ofs << "v" << i << " -> " << "v" << j << ";" << endl; 
        }
      ofs << "}" << endl;
      pcount += 1;
    } while (next_permutation(v.begin(), v.end()));
  }  
}

int
main () {
  gen_all("alldags", 5);
}

