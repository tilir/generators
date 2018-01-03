//------------------------------------------------------------------------------
//
// Naive pavings: programm to enumerate all tight pavings 
// refer to: https://oeis.org/A285357
//
//------------------------------------------------------------------------------
//
// This programm uses very naive approach: it just lists all possible pavings
// of MxN square by M+N-1 blocks for all possible blocks and outputs those
// results, which are checked to be paving
//
//------------------------------------------------------------------------------


#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "field.hpp"
#include "hind.hpp"

using std::cout;
using std::endl;
using std::fill;
using std::find_if;
using std::make_pair;
using std::map;
using std::max;
using std::minmax_element;
using std::next_permutation;
using std::pair;
using std::sort;
using std::stol;

size_t naive_gen(size_t N, size_t M) {
  size_t count = 0;

  // 1. enumerate possible block types
  //    (1 .. M-1) x (1 .. N-1) + 1 x N + 1 x M
  //   say 2:1 and 1:2 both have counter 2
  //
  //   btypecnts[2] == 2
  //   btypes[<2, 0>] = <2, 1>
  //   btypes[<2, 1>] = <1, 2>

  size_t nbtypes = max({N, M, (N-1)*(M-1)});
  vector<size_t> btypecnts(nbtypes+1);
  map<pair<size_t, size_t>, pair<size_t, size_t>> btypes {};

  btypes[make_pair(N, btypecnts[N])] = make_pair(N, 1);
  btypecnts[N] += 1;

  btypes[make_pair(M, btypecnts[M])] = make_pair(1, M);
  btypecnts[M] += 1;

  for (size_t horz = 1; horz < N; ++horz)
    for (size_t vert = 1; vert < M; ++vert) {
      btypes[make_pair(horz*vert, btypecnts[horz*vert])] = make_pair(horz, vert);
      btypecnts[horz*vert] += 1;
    }

  // 2. generate all type signatures of M+N-1 buckets
  //    with discrete allowed number of balls in each
  //
  //    n is allowed <=> btypecnts[n] != 0
  //    1 is always allowed (1x1 block)
  //
  //    say for M*N = 9 and allowed numbers 1, 2, 3, 4
  //    possible signatures:
  //
  //    1, 1, 1, 2, 4
  //    1, 1, 1, 3, 3
  //    1, 1, 2, 2, 3
  //    1, 2, 2, 2, 2
  //
  //    and all permutations

  size_t bsize = 1;
  for (size_t cnt = 2; cnt < btypecnts.size(); ++cnt) 
    if (btypecnts[cnt] > 0)
      bsize = cnt;

  size_t mback = (M + N - 1);
  size_t nballs = M*N;
  vector<size_t> bcnt (mback, 1);
  size_t excessballs = nballs - mback;

  // form minimal signature 1, 1 ... 1, r, n ... n
  // 2x2: 1, 1, 2
  // 2x3: 1, 1, 1, 3 
  // 3x3: 1, 1, 1, 2, 4
  // 4x3: 1, 1, 1, 1, 2, 6
  // 4x4: 1, 1, 1, 1, 1, 2, 9
  // suppose that r is always available number. As shown above it is always true 
  // for small numbers, but general case shall be proven separately, I think

  size_t curback = mback - 1;
  while(excessballs > 0) {
    if (excessballs > bsize - 1) {
      bcnt[curback] = bsize;
      excessballs -= (bsize - 1);
      assert (curback > 0);
      curback -= 1;      
    }
    else {
      assert (btypecnts[excessballs + 1] > 0);
      assert (bcnt[curback] == 1);
      bcnt[curback] += excessballs;
      excessballs = 0;
    }
  }

  do {
    if (bcnt.end() != find_if(bcnt.begin(), bcnt.end(), 
                              [&](size_t b) { return btypecnts[b] == 0; }))
      continue;

    do {
			// 3. for given signature generate all mixed-radix tuples
			//    say for 2, 1, 1, 2, 3
			//    btypecnts[1] == 1, btypecnts[2] == 2, btypecnts[3] == 2
			//    solutions are:
			//
			//    0, 0, 0, 0, 0
			//    0, 0, 0, 0, 1
			//    0, 0, 0, 1, 0
			//    0, 0, 0, 1, 1
			//    1, 0, 0, 0, 0
			//    1, 0, 0, 0, 1
			//    1, 0, 0, 1, 0
			//    1, 0, 0, 1, 1

      vector<size_t> bmix(mback, 0);

      for (;;) {
   			// 4. for mixed-radix tuple and signature fill field with btypes[<i, j>] blocks
		  	//    filter out non-pavings
			  //    filter-out non-tight pavings

        Field f(N, M);
        bool succ = true;

        for (size_t idx = 0; idx < mback; ++idx) {
          auto elt = btypes[make_pair(bcnt[idx], bmix[idx])];
          succ = f.put(elt.first, elt.second);
          if (!succ)
            break;
        }

        if (f.all() && f.tight()) {
          count += 1;
          cout << "Tight paving #" << count << ": ";
          f.dump(cout);
          cout << endl;
        }

        size_t digit = mback;

        while (digit > 0) {
          digit -= 1;
          if (bmix[digit] + 1 < btypecnts[bcnt[digit]]) {
            bmix[digit] += 1;
            break;
          }
          bmix[digit] = 0;
        }

        size_t bsum = 0;
        for (auto b: bmix) bsum += b;
        if (bsum == 0)
          break;
      }
    } while (next_permutation(bcnt.begin(), bcnt.end()));
  } while (next_break_of(M*N, M+N-1, bcnt.begin(), bcnt.end()));

  // 5. return result
  return count;
}


int
main(int argc, char **argv) {
  if ((argc != 3) || (stol(argv[1]) < 2) || (stol(argv[2]) < 2)) {
    cout << "Usage: " << argv[0] << " n, m" << endl;
    cout << "\tWhere n is horizontal size" << endl;
    cout << "\t      m is vertical size" << endl;
    cout << "Note: m and n shall be >= 2" << endl;
    return -1;
  }

  auto n = stol(argv[1]);
  auto m = stol(argv[2]);

  naive_gen(n, m);
}

