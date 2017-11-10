#include "libproperty/property.hpp"

#include <iostream>
#include <string>
#include <utility>
#include <cassert>

// plays perfectly nicely with templates
template <typename T>
class write_protected {
  using self = write_protected;

  constexpr T const& get_value() const
  {
    return data.value;
  }
  constexpr T const& set_value(T x)
  {
    if (writable) {
      data.value = std::move(x);
    }
    return data.value;
  }

public:
  /* - fields - */
  // enable writing?
  bool writable;

  // define the property called 'data'
  LIBPROPERTY_PROPERTY_WITH_STORAGE(T, data, get_value, set_value, self);

  /* - functions and methods - */
  // you need a constructor.
  constexpr write_protected(bool writable, T payload)
      : writable(writable), data(std::move(payload))
  {
  }

  friend std::ostream& operator<<(std::ostream& o, self const& x)
  {
    // use as normal - this calls the getter.
    return o << x.data << "\n";
  }
};

int main()
{
  write_protected<int> a{ true, 5 };
  assert(a.data == 5);
  // readable through the getter
  int const y = a.data;
  assert(y == 5);
  // writeable through the setter
  a.data = 7;
  assert(a.data == 7);
  // but only if writable is true:
  a.writable = false;
  a.data = 5; // ignored
  assert(a.data == 7);
}
