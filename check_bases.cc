#include <iostream>

#include "matgen.hpp"

constexpr unsigned dstart = 1;
constexpr unsigned dend = 10;

using Dom = UnsignedDomain<dstart, dend>;

using bitstring_t = BitString<Dom>;
using subsets_t = SubSets<Dom>;

using construction_t = std::vector<subsets_t>;
using extension_t = std::map<int, subsets_t>;

void indep() {
  std::cout << "Checking independent sets:" << std::endl;
  std::cout << std::boolalpha;

  // examples from slides:
  subsets_t I1{1, 3, 12, 13};
  I1.dump(std::cout);
  std::cout << ": ";
  std::cout << I1.check_indep() << std::endl;

  subsets_t I2{1, 2, 3, 12};
  I2.dump(std::cout);
  std::cout << ": ";
  std::cout << I2.check_indep() << std::endl;

  subsets_t I3{1, 2, 3, 12, 13, 123};
  I3.dump(std::cout);
  std::cout << ": ";
  std::cout << I3.check_indep() << std::endl;

  subsets_t I4;
  I4.fill(3, 7);
  subsets_t I4exc{123, 234, 345, 456};
  I4.exclude(I4exc.begin(), I4exc.end());
  I4.dump(std::cout);
  std::cout << ": ";
  std::cout << I4.check_indep() << std::endl;

  subsets_t I5;
  I5.fill(2, 7);
  subsets_t I5add{123, 234, 345, 456};
  I5.assign(I5add.begin(), I5add.end());
  I5.dump(std::cout);
  std::cout << ": ";
  std::cout << I5.check_indep() << std::endl;
}

void bases() {
  std::cout << "Checking bases:" << std::endl;
  std::cout << std::boolalpha;

  // examples from slides:
  subsets_t B1{12, 13};
  B1.dump(std::cout);
  std::cout << ": ";
  std::cout << B1.check_bases() << std::endl;

  subsets_t B2{12, 34};
  B2.dump(std::cout);
  std::cout << ": ";
  std::cout << B2.check_bases() << std::endl;

  subsets_t B3{123, 124};
  B3.dump(std::cout);
  std::cout << ": ";
  std::cout << B3.check_bases() << std::endl;

  subsets_t B4;
  B4.fill_exact(3, 7);
  subsets_t B4exc{123, 234, 345, 456};
  B4.exclude(B4exc.begin(), B4exc.end());
  B4.dump(std::cout);
  std::cout << ": ";
  std::cout << B4.check_bases() << std::endl;

  subsets_t B5{123, 234, 345, 456};
  B5.dump(std::cout);
  std::cout << ": ";
  std::cout << B5.check_bases() << std::endl;
}

int main() {
#if defined(INDEP)
  indep();
#elif defined(BASES)
  bases();
#else
#error "you shall define"
#endif
}
