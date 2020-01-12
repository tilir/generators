//-----------------------------------------------------------------------------
//
//  Integral domains
//
//  Integral domain encodes integral diapasone [A, B]
//  Say Idom<unsigned, 1, 7> encodes range {1, 2, 3, 4, 5, 6}
//
//-----------------------------------------------------------------------------
//
// This file is licensed after GNU GPL v3
//
//-----------------------------------------------------------------------------

#pragma once

#include <cassert>
#include <string>

using std::string;
using std::to_string;

template <typename T, T start_, T fin_> struct Idom {
  T val_;
  Idom(T val = start_) : val_(val) {
    assert(val < fin_);
    assert(val >= start_);
  }
  operator T() const { return val_; }

  using type = T;
  static constexpr T start = start_;
  static constexpr T fin = fin_;
};

template <unsigned start_, unsigned fin_>
using UnsignedDomain = Idom<unsigned, start_, fin_>;

template <char start_, char fin_> using CharDomain = Idom<char, start_, fin_>;
