//-----------------------------------------------------------------------------
// matgen.cc -- implementation of Knuth matroid generation algorithm
//
// Discrete Mathematics 12, 1975, pp. 341-358
//
//-----------------------------------------------------------------------------
//
// This file is licensed after GNU GPL v3
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "matgen.hpp"

// domain: [dstart .. dend)
constexpr unsigned dstart = 0;
constexpr unsigned dend = 10;

using Dom = UnsignedDomain<dstart, dend>;

using bitstring_t = BitString<Dom>;
using subsets_t = SubSets<Dom>;

using construction_t = std::vector<subsets_t>;
using extension_t = std::map<int, subsets_t>;

construction_t create_matroid(const extension_t &ext) {
  construction_t ret;
  bitstring_t zero_elt;
  subsets_t curset{zero_elt};
  ret.push_back(curset);

  for (;;) {
    unsigned r = ret.size();
    subsets_t newsets;

    // iterating for all sets of prev one
    for (auto bs : curset) {
      // adding to each one-by-one all domain elts
      auto bsc = bs;
      for (auto delt = dstart; delt != dend; ++delt) {
        if (bsc.extend(delt))
          newsets.extend(bsc);
        bsc = bs;
      }
    }

#if VISUALIZE
    std::cout << "rang: " << r << std::endl;
    newsets.dump(std::cout);
    std::cout << std::endl;
#endif

    auto eit = ext.find(r);
    if (eit != ext.end()) {
      newsets.assign(eit->second.cbegin(), eit->second.cend());

#if VISUALIZE
      std::cout << "after ext: " << std::endl;
      newsets.dump(std::cout);
      std::cout << std::endl;
#endif
    }

    // this is quadratic step: eliminating sets by pairwise check
    newsets.eliminate(curset);
#if VISUALIZE
    std::cout << "after elim: " << std::endl;
    newsets.dump(std::cout);
    std::cout << std::endl;
#endif

    curset = newsets;
    ret.push_back(newsets);

    if (newsets.size() == 1)
      break;
  }

  return ret;
}

int main() {
  // extender is from pi, as in article
  subsets_t extender_2{134, 159, 256, 358, 379, 238};
  extender_2.dump(std::cout);
  std::cout << std::endl;

  extension_t e;
  e[2] = extender_2;
  auto csets = create_matroid(e);

  std::cout << "Closed sets by construction:" << std::endl;
  for (auto s : csets) {
    s.dump(std::cout);
    std::cout << std::endl;
  }
}
