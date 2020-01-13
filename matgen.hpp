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

#include "comb.hpp"
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

  unsigned size() const { return __builtin_popcount(s_); }

  template <typename Fwd> void assign(Fwd start, Fwd fin) {
    for (auto it = start; it != fin; ++it)
      extend(*it);
  }

  // inclusion check
  bool includes(DomT delt) { return ((s_ | (1 << delt)) == s_); }

  // extend {1, 3, 4} with, say, 7
  bool extend(DomT delt) {
    if (includes(delt))
      return false;
    s_ |= (1 << delt);
    return true;
  }

  bool remove(DomT delt) {
    if (!includes(delt))
      return false;
    s_ &= ~(1 << delt);
    return true;
  }

  void clear() { s_ = 0; }

  bool contains(const BitString &rhs) const {
    return ((s_ & rhs.s_) == rhs.s_) && ((s_ | rhs.s_) == s_);
  }

  // B1 - B2
  void operator-=(const BitString &rhs) { s_ = (s_ & ~rhs.s_); }

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

private:
  struct Iter {
    storage_t s_;

  public:
    Iter(storage_t s = 0) : s_{s} {}
    DomT operator*() {
      assert(s_ != 0);
      return __builtin_ctz(s_);
    }
    Iter &operator++() {
      s_ = (s_ & (s_ - 1));
      return *this;
    }
    Iter operator++(int) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }
    bool operator==(Iter rhs) { return rhs.s_ == s_; }
    bool operator!=(Iter rhs) { return !(*this == rhs); }
  };

public:
  Iter begin() { return Iter{s_}; }
  Iter end() { return Iter{}; }
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
  SubSets(std::initializer_list<unsigned> il) {
    for (auto nxt : il) {
      std::vector<DTT> nums;
      while (nxt > 0) {
        auto elt = nxt % DomT::fin;
        assert(elt >= DomT::start);
        nums.push_back(elt);
        nxt /= DomT::fin;
      }
      BST bs;
      bs.assign(nums.begin(), nums.end());
      b_.insert(bs);
    }
  }

  // fill with all subsets of size sz
  // fill_exact(3, 7): 123, 124, ... 456
  void fill_exact(unsigned sz, DomT fin = DomT::fin) {
    all_comb ac(fin - DomT::start, sz);
    do {
      BST bs;
      for (auto it = ac.begin(); it != ac.end(); ++it)
        bs.extend(*it + DomT::start);
      b_.insert(bs);
    } while (ac.next_comb());
  }

  // fill with all subsets up to size sz
  void fill(unsigned sz, DomT fin = DomT::fin) {
    for (int i = 1; i <= sz; ++i)
      fill_exact(i, fin);
  }

  void extend(BST b) { b_.insert(b); }

  template <typename Fwd> void assign(Fwd start, Fwd fin) {
    for (auto it = start; it != fin; ++it)
      b_.insert(*it);
  }

  template <typename Fwd> void exclude(Fwd start, Fwd fin) {
    for (auto it = start; it != fin; ++it)
      b_.erase(*it);
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

  // naive independence check
  bool check_indep() const;

  // naive weak base exchange check
  bool check_bases() const;
};

template <typename DomT> bool SubSets<DomT>::check_indep() const {
  for (auto it = b_.begin(); it != b_.end(); ++it)
    for (auto it2 = it; it2 != b_.end(); ++it2) {
      if (it->size() > it2->size()) {
        auto bigger = *it;
        auto smaller = *it2;
        bool ok = false;
        for (auto elt : bigger) {
          if (smaller.includes(elt))
            continue;
          auto augmented = smaller;
          augmented.extend(elt);
          if (contains(augmented)) {
            ok = true;
            break;
          }
        }
        if (!ok) {
          std::cout << std::endl;
          std::cout << "Indep check failed: ";
          bigger.dump(std::cout);
          std::cout << " ";
          smaller.dump(std::cout);
          std::cout << std::endl;
          return false;
        }
      }
    }
  return true;
}

template <typename DomT> bool SubSets<DomT>::check_bases() const {
  for (auto it = b_.begin(); it != b_.end(); ++it)
    for (auto it2 = std::next(it); it2 != b_.end(); ++it2) {
      if (it->size() != it2->size())
        return false;
      auto B1 = *it;
      auto B2 = *it2;
      if (B1 == B2)
        continue;

      auto C = B1, D = B2;
      C -= B2;
      D -= B1;

      // for all x from B1-B2 check that exists y from B2-B1,
      // such that B1 - x + y contained in B
      for (auto x : C) {
        bool found = false;
        for (auto y : D) {
          auto AUG = B1;
          AUG.remove(x);
          AUG.extend(y);
          if (b_.find(AUG) != b_.end()) {
            found = true;
            break;
          }
        }
        if (!found) {
          std::cout << std::endl;
          std::cout << "Base check failed: ";
          B1.dump(std::cout);
          std::cout << " ";
          B2.dump(std::cout);
          std::cout << " ";
          C.dump(std::cout);
          std::cout << " ";
          D.dump(std::cout);
          std::cout << "; elem: " << x << std::endl;
          return false;
        }
      }
    }
  return true;
}
