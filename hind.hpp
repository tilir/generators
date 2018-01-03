//------------------------------------------------------------------------------
//
// Hindenburg algorithm, rewritten in C++ way
// see also: 7.2.1.4H in TAOCP
//
//------------------------------------------------------------------------------
//
// This file implements next_break_of(n, m, begin, end) step
// just like std::next_permuitation do so for permutations
//
//------------------------------------------------------------------------------

#ifndef HIND_GUARD_
#define HIND_GUARD_

#include <cassert>

template <typename Iter>
bool next_break_of(size_t n, size_t m, Iter start, Iter end) {
  assert(n >= 2);
  assert(m >= 2);

  auto &last = *--end;
  auto &pre_last = *--end;

  if (pre_last < last - 1) {
    last -= 1;
    pre_last += 1;
    return true;
  }

  size_t j = 3;
  auto itj = --end;
  size_t s = last + pre_last - 1;

  while ((j <= m) && (*itj >= last - 1)) {
    s = s + *itj;
    j += 1;
    itj--;
  }

  if (j > m)
    return false;

  size_t x = *itj + 1;
  *itj = x;
  itj++;
  j -= 1;

  while (j > 1) {
    *itj = x;
    s -= x;
    j -= 1;
    itj++;
  }

  last = s;
  return true;
}

#endif
