//------------------------------------------------------------------------------
//
//  Field (2D) that is a rectangle to be paved in
//
//------------------------------------------------------------------------------
//
// Initial field (x shows current position)
//
//     N
//  <----->
//  x****** ^
//  ******* | M
//  ******* v
//
//  After adding 1x5 block #1
//
//  11111x-
//  -------
//  -------
//
//  After adding 3x2 block #2
//
//  1111122
//  x----22
//  -----22
//
//  After adding 2x3 block #3
//
//  1111122
//  333x-22
//  333--22
//
//  After adding 2x1 block #4
//
//  1111122
//  3334x22
//  3334-22
//
// After adding 1x1 block #5 and 1x1 block #6
//
//  1111122
//  3334522
//  3334622
//  x
//
// Field is now filled: 11111 22 | 333 4 5 22 | 333 4 6 22
//
// It is paving, but it is not tight paving because
// some coordinates are not used
//
//------------------------------------------------------------------------------

#ifndef FIELD_GUARD_
#define FIELD_GUARD_

#include <iostream>
#include <cassert>
#include <vector>

using std::fill;
using std::find_if;
using std::ostream;
using std::vector;

template <typename V> static inline bool all_set(V x) {
  return (x.end() ==
          find_if(x.begin(), x.end(), [](size_t b) { return (b == 0); }));
}

class Field {
  size_t N;
  size_t M;
  vector<size_t> fld_;
  vector<size_t> hcoord_;
  vector<size_t> vcoord_;
  size_t vpos_ = 0;
  size_t hpos_ = 0;
  size_t curnum_ = 1;

  // helper: promote vpos_ and hpos_ to unoccupied
  // returns n of promoted cells
  size_t promote() {
    size_t res = 0;
    while ((vpos_ * N + hpos_ < M * N) && fld_[vpos_ * N + hpos_]) {
      hpos_ += 1;
      res += 1;
      if (hpos_ == N) {
        hpos_ = 0;
        vpos_ += 1;
      }
    }
    return res;
  }

  // helper: pure horizontal block (1 x len)
  bool put_horz(size_t len, size_t num) {
    assert(len > 0);
    assert(hpos_ + len <= N);

    if (vpos_ * N + hpos_ + len - 1 > M * N)
      return false;

    for (size_t idx = 0; idx != len; ++idx) {
      if (fld_[vpos_ * N + hpos_ + idx] > 0)
        return false;
      fld_[vpos_ * N + hpos_ + idx] = num;
    }

    promote();
    return true;
  }

  // helper: pure vertical block (len x 1)
  bool put_vert(size_t len, size_t num, bool check_promotion) {
    assert(len > 0);

    // we can not assert it: we are breaking block into vertical parts
    if (vpos_ + len > M)
      return false;

    if ((vpos_ + len - 1) * N + hpos_ > M * N)
      return false;

    for (size_t idx = 0; idx != len; ++idx) {
      if (fld_[(vpos_ + idx) * N + hpos_] > 0)
        return false;
      fld_[(vpos_ + idx) * N + hpos_] = num;
    }

    size_t np = promote();

    // when placing by parts, we need to check promotion
    if (check_promotion && (np > 1))
      return false;
    return true;
  }

public:
  Field(size_t horz, size_t vert)
      : N(horz), M(vert), fld_(M * N, 0), hcoord_(N - 1, 0), vcoord_(M - 1, 0) {
  }

  void reset() {
    fill(fld_.begin(), fld_.end(), 0);
    fill(hcoord_.begin(), hcoord_.end(), 0);
    fill(vcoord_.begin(), vcoord_.end(), 0);
    hpos_ = 0;
    vpos_ = 0;
    curnum_ = 1;
  }

  bool all() { return all_set(fld_); }

  bool tight() { return all_set(hcoord_) && all_set(vcoord_); }

  // Easiest way to determine vtype is to use vertical lengths of current num
  //
  // 1 2 2 2 3
  // 1 4 5 6 3
  // 1 4 5 7 3
  //
  // is tight paving but not a vtype, because it has on level 2
  // adjacent curnums 4 and 5  with same vlen and vends
  //
  // but
  //
  // 1 2 2 2 3
  // 1 4 5 6 3
  // 1 4 7 6 3
  //
  // is a vtype.
  //
  // Btw, it is one of six proper 5x3 vtypes, others five are:
  // 12345|16345|16375
  // 12345|12645|17775
  // 12345|12365|17365
  // 12334|12564|17764
  // 12234|15634|15774
  //
  bool vtype() {
    assert(all() && tight());
    assert(curnum_ - 1 == M + N - 1);
    vector<size_t> vlens(curnum_, 0);
    vector<size_t> vends(curnum_, 0);
    for (size_t y = 0; y < M; ++y)
      for (size_t x = 0; x < N; ++x) {
        auto cur = y * N + x;
        auto ncur = fld_[cur];
        if (vlens[ncur] == 0) {
          for (size_t test = 0; test < M; ++test) {
            if (ncur == fld_[test * N + x]) {
              vlens[ncur] += 1;
              vends[ncur] = test;
            }
          }
          if (x > 0) {
            auto nprev = fld_[cur - 1];
            assert(ncur != nprev);
            if ((vlens[ncur] == vlens[nprev]) && (vends[ncur] == vends[nprev]))
              return false;
          }
        }
      }
    return true;
  }

  vector<size_t> vtype_signature() {
    vector<size_t> retval(N);
    for (size_t x = 0; x < N; ++x) {
      size_t next = 1;
      for (size_t y = 1; y < M; ++y) 
        if (fld_[y*N + x] != fld_[(y-1)*N + x])
          next += 1;
      retval[x] = next;
    }
    return retval;
  }

  void dump(ostream &os) {
    for (size_t x = 0; x < M; ++x) {
      for (size_t y = 0; y < N; ++y)
        os << fld_[x * N + y];
      if (x != M - 1)
        os << "|";
    }
  }

  bool put(size_t hlen, size_t vlen) {
    assert((hlen >= 1) && (vlen >= 1));
    size_t oldhpos = hpos_;
    size_t oldvpos = vpos_;
    bool res;

    if (hpos_ + hlen > N)
      return false;

    if (vpos_ + vlen > M)
      return false;

    if (vlen == 1) {
      res = put_horz(hlen, curnum_);
      if (!res)
        return false;
    } else {
      for (int idx = 0; idx != hlen; ++idx) {
        res = put_vert(vlen, curnum_, (idx != hlen - 1));
        if (!res)
          return false;
      }
    }

    if (oldhpos + hlen < N)
      hcoord_[oldhpos + hlen - 1] += 1;

    if (oldvpos + vlen < M)
      vcoord_[oldvpos + vlen - 1] += 1;

    curnum_ += 1;
    return res;
  }
};

#endif
