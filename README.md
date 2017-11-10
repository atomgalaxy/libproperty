Libproperty
===========

An experimental design for a property library for C++.

Usage:
------

* See [test](tests/examples.cpp) for examples.

### Quick example: ###

```c++
#include "libproperty/property.hpp"

#include <iostream>
#include <string>
struct my_class {

  int const& my_getter() const { return property.value; }
  int const& my_setter(std::string const& x) {
    return property.value = atoi(x.c_str());
  }
  LIBPROPERTY_PROPERTY_WITH_STORAGE(
      int, property, my_getter, my_setter, my_class);
};

int main() {
  my_class a;
  a.property = "5";
  std::cout << "should print 8: " << a.property + 3 << '\n';
  std::cout << "sizeof(a): " << sizeof(a)
            << " == sizeof(int): " << sizeof(int) << '\n';
}
```

Public interface:
-----------------
For the public interface, see [the main source file](libproperty/property.hpp).


Other nifty features:
---------------------

Classes that use properties don't inflate (classes that just use the
getter/setter capabilities do inflate - one needs at least one byte per class).

   auto x = y.property;

does not work - the copy constructor is private, as are assignment operators for
the property type. It is therefore hard to copy it out of the class, where
accessing it would cause extreme havoc (don't do that!).


Authors
-------

The first version was started by
[Viktor Korsun](https://github.com/bitekas/properties).

He have a full-length talk about it at CppCon 2015, which is when
[Gašper Ažman](https://github.com/atomgalaxy/properties) re-wrote it overnight
and gave a lightning talk improving on Viktor's concept the next day.


Acknowledgements
----------------

The current library also owes some conceptual ideas about metaprogrammimng to
Louis Dionne, our dear author of boost.hana.
