#include "libproperty/property.hpp"

#include <iostream>
#include <string>
#include <cassert>
struct my_class {

  int const& my_getter() const { return property.value; }
  int const& my_setter(std::string const& x) {
    return property.value = atoi(x.c_str());
  }
  LIBPROPERTY_PROPERTY(int, property, my_getter, my_setter, my_class);
};

int main() {
  my_class a;
  a.property = "5";
  assert(a.property == 5);
}
