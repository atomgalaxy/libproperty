Libproperty
===========

An experimental design for a property library for C++.

Usage:
------

* See [test](tests/examples.cpp) for examples.

### Quick example: ###

```
 
```

```c++
#include "libproperty/property.hpp"

#include <iostream>
#include <string>
struct my_class {

  int const& my_getter() const { return property.value; }
  int const& my_setter(std::string const& x) {
    return property.value = atoi(x.c_str());
  }
  LIBPROPERTY_PROPERTY((int), property, my_getter, my_setter, my_class);
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

### Reserved names
`libproperty` may create any number of static member functions and types inside
your class where you use the macros.

All names of objects created inside your class begin with `_libproperty__`.

### Guarantees:
- The *layout* of your type will be exactly the same as if you defined a
  variable of type `T` instead, where `T` is the `type` parameter to the
  definition macro.
- The only new member of your class has the name `name`.
- The macros may not create any `virtual` functions.

This means you can rely on the layout of your class. The layout is guaranteed to
be the same as the `type` you supply to the creation macro.

### Macros:

```c++
LIBPROPERTY_WRAPPED((type), name, host_type); // recommended
LIBPROPERTY_PROPERTY((type), name, getter_name, setter_name, host_type);
LIBPROPERTY_PROPERTY2((type), name, getter_addr, setter_addr, host_type);
LIBPROPERTY_EMPTY_PROPERTY((type), name, getter_name, setter_name, host_type);
LIBPROPERTY_EMPTY_PROPERTY2((type), name, getter_addr, setter_addr, host_type);
```
Defines a property with name `name` inside `host_type` that holds a `type`. The
value of the property is accessible using `name.value` from within the class.

Must be used within `host_type` definition.

The getter and setter must be methods of the host type.

The `getter_name` and `setter_name` parameters are just names of methods. They
are short-hands for when the getter and setter are not templates.

The `getter_addr` and `setter_addr` are the addresses of the `getter` and
`setter` member functions. Use them when you need to instantiate the getter and
setter templates by casting them to a member function pointer of the required
type.

The `host` type is the name of your class. Since these macros must be used
inside the class definition, you may omit all template parameters.

### `rw_property`

The macros above create a `rw_property` instance. Each one has a unique type
that carries the type, the host, and a `name`-derived type tag. These are used
to power the library machinery. It only has one member: `T value`, and it is
private. However, since the host class is declared a `friend` of the property
class, the host's methods are allowed access to the value member.

Its copy and move constructors, default constructor, destructor, and copy and
move-assignment operators are defined private, thus only permitting the host
class to copy the object.

The only other parts of the interface of note are the conversion operator to
anything the getter produces, and an assignment operator from anything that the
setter will accept as a parameter.

TODO: write examples for all of the above-mentioned corner cases.

Other nifty features:
---------------------

### Zero-overhead guarantee
Classes that use properties don't inflate (classes that just use the
getter/setter capabilities do inflate - one needs at least one byte per class).

### Safe.
```
   auto x = y.property;
```

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


TODO
----

### missing tests
- move / copy actually does what it's supposed to
- getter and setter are members, not functions
 - the value category of data is correct and gets correctly forwarded to the
   setter
- test where `type` and getters are templated methods (to see macro problems)
- test regular inheritance hierarchies
  - properties in base
  - properties in derived
  - properties in sibling (top and bottom sibling)
  - crtp
- test virtual inheritance hierarchies

#### Negative tests:
- auto `x = y.prop` doesn't compile.

### Missing features:
- noexcept / sfinae / constexpr correctness
  - ask Vittorio for some help, most likely
- in-place constructors? It *is* a container...
- constexpr is difficult because `reinterpret_cast` doesn't work constexpr
