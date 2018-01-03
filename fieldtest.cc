#include "field.hpp"

int
test_field() {
  Field f(4, 3);
  assert(f.put(3, 1));
  assert(!f.put(3, 1));
  f.reset();
  assert(f.put(3, 1));
  assert(f.put(1, 3));
  assert(f.put(2, 1));
  assert(!f.put(1, 3));
  f.reset();
  assert(f.put(3, 1));
  assert(f.put(1, 3));
  assert(!f.put(4, 1));
  f.reset();
  assert(f.put(3, 1));
  assert(f.put(1, 3));
  assert(f.put(3, 1));
  assert(f.put(1, 1));
  assert(!f.all());
  assert(f.put(2, 1));
  assert(f.all());
  assert(!f.tight());

  Field fvert(3, 4);
  assert(fvert.put(1, 2));
  assert(fvert.put(1, 1));
  assert(fvert.put(1, 3));
  assert(fvert.put(1, 1));
  assert(fvert.put(2, 2));
  assert(!fvert.all());
  assert(fvert.put(1, 1));
  assert(fvert.all());
  assert(fvert.tight());

  Field f2(3, 3);
  assert(f2.put(1, 1));
  assert(f2.put(1, 1));
  assert(f2.put(1, 1));
  assert(f2.put(1, 2));
  assert(f2.put(2, 2));
  assert(f2.all());
  assert(!f2.tight());
  f2.reset();
  assert(f2.put(1, 1));
  assert(f2.put(1, 1));
  assert(!f2.put(2, 2));
  f2.reset();
  assert(f2.put(1, 1));
  assert(f2.put(1, 2));
  assert(f2.put(1, 1));
  assert(!f2.put(2, 2));
}


int
main() {
  test_field();
}

