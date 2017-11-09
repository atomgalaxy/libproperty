#ifndef LIBPROPERTY_PROPERTY_HPP_INCLUDED
#define LIBPROPERTY_PROPERTY_HPP_INCLUDED

/*
The MIT License (MIT)

Copyright (c) 2015, 2017 Gašper Ažman

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "property_impl.hpp"

#include <utility> // for std::forward

// only call in class scope!
#define LIBPROPERTY_PROPERTY_WITH_STORAGE(type, name, getter, setter, host)    \
  struct LIBPROPERTY__TAG_NAME(name);                                          \
  ::libproperty::rw_property<type,                                             \
      host,                                                                    \
      ::libproperty::meta::value_<&host::getter>,                              \
      ::libproperty::meta::value_<&host::setter>,                              \
      ::libproperty::meta::type_<host::LIBPROPERTY__TAG_NAME(name)>>           \
      name;                                                                    \
  auto static constexpr LIBPROPERTY__FUNC_NAME(                                \
      decltype(::libproperty::impl::tag_of(name)))                             \
  {                                                                            \
    return &host::name;                                                        \
  }                                                                            \
  /* so that we can have the missing semicolon... */                           \
  static_assert(true)

// end define
#define LIBPROPERTY_PROPERTY(name, host, getter, setter)                       \
  LIBPROPERTY_PROPERTY_WITH_STORAGE(char, name, host, getter, setter)

namespace libproperty {

template <typename ValueType,
    typename Host,
    typename Getter,
    typename Setter,
    typename TypeTag>
struct rw_property {
  using getter = Getter;
  using setter = Setter;
  using tag = TypeTag;
  using host = Host;
  using value_type = ValueType;

  friend host;

  constexpr operator decltype(auto)() const
  {
    namespace pi = ::libproperty::impl;
    return (pi::get_host<host>(this).*getter::value)();
  }

  template <typename X>
  decltype(auto) operator=(X&& x)
  {
    namespace pi = ::libproperty::impl;
    return (pi::get_host<host>(this).*setter::value)(std::forward<X>(x));
  }

  /* TODO: write comment that explains you don't have to use 'value' and that
   * it's not required to be related to the getter and setter at all. */

private:           // for the use of host, not for nobody's!
  ValueType value; // possibly unused.

  /// disallow copying for non-friend users of the class - this doesn't have a
  /// value, but if copied, it can get really, really bad (stack corruption).
  rw_property() = default;
  rw_property& operator=(rw_property const&) = default;
  rw_property(rw_property const&) = default;
  ~rw_property() = default;
  rw_property& operator=(rw_property&&) = default;
  rw_property(rw_property&&) = default;
};

} // property

#endif
