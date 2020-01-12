//-----------------------------------------------------------------------------
// matgen.hpp -- header to contain useful classes in matroid generation
//
// When generating matroids, two useful conceptions are:
//   * set of domain elements
//   * set of such sets
//
// Latter emulates virtually everything: independent sets, bases, circuits,
// closed sets, hyperplanes, etc...
//
// Set of domain elements is to be better emulated by number
//
//-----------------------------------------------------------------------------
//
// This file is licensed after GNU GPL v3
//
//-----------------------------------------------------------------------------

#include <map>
#include <set>
#include <vector>

#include "idomain.hpp"

// to be changed
using storage_t = unsigned;

template <typename DomT> class BitString final {
  storage_t s_ = 0;
  using DTT = typename DomT::type;

  // if this is not the case, then we shall invent better storage
  static_assert(DomT::fin < sizeof(storage_t) * CHAR_BIT);

public:
  BitString(storage_t s = 0) : s_{s} {}

  // like {1, 3, 4}
  explicit BitString(std::initializer_list<DTT> il) {
    assign(il.begin(), il.end());
  }

  operator storage_t() const { return s_; }

  template <typename Fwd> void assign(Fwd start, Fwd fin) {
    for (auto it = start; it != fin; ++it)
      extend(*it);
  }

  // extend {1, 3, 4} with, say, 7
  bool extend(DomT delt) {
    if ((s_ | (1 << delt)) == s_)
      return false;
    s_ |= (1 << delt);
    return true;
  }

  void clear() { s_ = 0; }

  bool contains(const BitString &rhs) const {
    return ((s_ & rhs.s_) == rhs.s_) && ((s_ | rhs.s_) == s_);
  }

  std::ostream &dump(std::ostream &os) const {
    if (s_ == 0) {
      os << "{}";
      return os;
    }

    for (auto delt = DomT::start; delt != DomT::fin; ++delt)
      if ((s_ & (1 << delt)) == (1 << delt))
        os << delt;
    return os;
  }
};

template <typename DomT> class SubSets final {
  using DTT = typename DomT::type;
  using BST = BitString<DomT>;
  std::set<BST> b_;

public:
  SubSets() = default;

  // one-element set
  SubSets(BST b) { extend(b); }

  // very special case: like {134, 159, 256}
  // only for zero-based domains
  SubSets(std::initializer_list<unsigned> il) {
    static_assert(DomT::start == 0);
    for (auto nxt : il) {
      std::vector<DTT> nums;
      while (nxt > 0) {
        nums.push_back(nxt % DomT::fin);
        nxt /= DomT::fin;
      }
      BST bs;
      bs.assign(nums.begin(), nums.end());
      b_.insert(bs);
    }
  }

  void extend(BST b) { b_.insert(b); }

  template <typename Fwd> void assign(Fwd start, Fwd fin) {
    for (auto it = start; it != fin; ++it)
      b_.insert(*it);
  }

  void clear() { b_.clear(); }

  auto begin() { return b_.begin(); }
  auto end() { return b_.end(); }

  auto cbegin() const { return b_.cbegin(); }
  auto cend() const { return b_.cend(); }

  auto size() const { return b_.size(); }

  std::ostream &dump(std::ostream &os) const {
    os << "[ ";
    for (auto belt : b_) {
      belt.dump(os);
      os << " ";
    }
    os << "]";
    return os;
  }

  bool contains(BST elt) const {
    if (b_.find(elt) != b_.end())
      return true;

    // costly step, need custom search tree?
    for (auto bst : b_)
      if (bst.contains(elt))
        return true;

    return false;
  }

  void eliminate(const SubSets &cs) {
    bool eliminated = true;

    while (eliminated) {
      eliminated = false;
      std::vector<BST> velts(b_.begin(), b_.end());
      for (auto it = velts.begin(); it != velts.end(); ++it) {
        BST A = *it;
        // now we shall decide if we want to add A to nb_
        // or to change A to zero set and change B to (A or B)
        for (auto it2 = std::next(it); it2 != velts.end(); ++it2) {
          BST B = *it2;
          BST C = A & B;
          if (!cs.contains(C)) {
            BST D = A | B;
            *it = D;
            *it2 = D;
            eliminated = true;
          }
        }
      }

      b_.clear();
      b_.insert(velts.begin(), velts.end());
    }
  }
};
