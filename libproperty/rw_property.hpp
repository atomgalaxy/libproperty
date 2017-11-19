#ifndef INCLUDED_LIBPROPERTY_RW_PROPERTY_HPP
#define INCLUDED_LIBPROPERTY_RW_PROPERTY_HPP

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

#include <type_traits>
#include <utility> // for std::forward

#define LIBPROPERTY_PROPERTY2(type, name, getter, setter, host)                \
  struct LIBPROPERTY__TAG_NAME(name);                                          \
  ::libproperty::rw_property<LIBPROPERTY__PARENTHESIZED_TYPE type,             \
      host,                                                                    \
      ::libproperty::meta::type_<host::LIBPROPERTY__TAG_NAME(name)>>           \
      name;                                                                    \
  auto static constexpr _libproperty__get_metadata(                            \
      decltype(::libproperty::impl::tag_of(name)))                             \
  {                                                                            \
    namespace pi = ::libproperty::impl;                                        \
    namespace pm = ::libproperty::meta;                                        \
    return pi::rw_property_metadata<pm::value_<&host::name>,                   \
        pm::value_<getter>,                                                    \
        pm::value_<setter>,                                                    \
        host>{};                                                               \
  }                                                                            \
  static_assert(true)

// only call in class scope!
#define LIBPROPERTY_PROPERTY(type, name, getter, setter, host)                 \
  LIBPROPERTY_PROPERTY2(type, name, &host::getter, &host::setter, host)

// end define
#define LIBPROPERTY_EMPTY_PROPERTY(name, getter, setter, host)                 \
  LIBPROPERTY_PROPERTY((char), name, getter, setter, host)

#define LIBPROPERTY_EMPTY_PROPERTY2(name, getter, setter, host)                \
  LIBPROPERTY_PROPERTY2((char), name, getter, setter, host)

namespace libproperty {

template <typename T, typename Host, typename Tag>
struct rw_property {
  using tag = Tag;
  using host = Host;
  using value_type = T;

  friend host;

  constexpr operator decltype(auto)() const
  {
    namespace pi = ::libproperty::impl;
    auto constexpr getter = metadata().getter;
    return (pi::get_host<host>(*this).*getter)();
  }

  template <typename X>
  decltype(auto) operator=(X&& x) // I don't want to say it 3 times...
  {
    namespace pi = ::libproperty::impl;
    auto constexpr setter = metadata().setter;
    return (pi::get_host<host>(*this).*setter)(std::forward<X>(x));
  }

private:   // for the use of host, not for anyone else!
  T value; // possibly unused.

  /// disallow copying for non-friend users of the class - this doesn't have a
  /// value, but if copied, it can get really, really bad (stack corruption).
  constexpr rw_property() = default;
  constexpr rw_property(rw_property const&) = default;
  constexpr rw_property(rw_property&&) = default;
  ~rw_property() = default;
  constexpr rw_property& operator=(rw_property const&) = default;
  constexpr rw_property& operator=(rw_property&&) = default;

  // value-construction
  constexpr rw_property(T&& x) noexcept(std::is_nothrow_move_constructible_v<T>)
      : value(std::move(x))
  {
  }
  constexpr rw_property(T const& x) noexcept(
      std::is_nothrow_copy_constructible_v<T>)
      : value(x)
  {
  }

  static constexpr auto metadata()
  {
    return host::_libproperty__get_metadata(tag{});
  }
};

template <typename T, typename H, typename Tag>
struct is_property<rw_property<T, H, Tag>> : std::true_type {
};

} // property

#endif
