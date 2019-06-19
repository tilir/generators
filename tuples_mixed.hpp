//------------------------------------------------------------------------------
//
// this file implements all mixed mode tuples
//
// Details may be found in Knuth, algorithm M from 7.2.1.1 (vol 4A)
//
//------------------------------------------------------------------------------
//
// This file is licensed after LGPL v3
// Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
//
//------------------------------------------------------------------------------

#include <vector>

class MixedModeTuple {
  int len_;
  std::vector<int> borders_, result_;

public:
  template <typename It>
  MixedModeTuple(It start, It fin)
      : len_(fin - start), borders_(start, fin), result_(len_) {}

  bool next_tuple() {
    /* M3 */
    int j = len_;

    /* M4 */
    while ((j > 0) && (result_[j - 1] == borders_[j - 1] - 1)) {
      result_[j - 1] = 0;
      j = j - 1;
    }

    if (j == 0)
      return false;

    result_[j - 1] += 1;
    return true;
  }

  auto begin() { return result_.begin(); }
  auto end() { return result_.end(); }
};
