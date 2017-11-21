#ifndef INCLUDED_LIBPROPERTY_WRAPPER_HPP
#define INCLUDED_LIBPROPERTY_WRAPPER_HPP

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

#define LIBPROPERTY_WRAP(type, name, host)                                     \
  struct LIBPROPERTY__TAG_NAME(name);                                          \
  ::libproperty::wrapper<LIBPROPERTY__PARENTHESIZED_TYPE type,                 \
      host,                                                                    \
      host::LIBPROPERTY__TAG_NAME(name)>                                       \
      name;                                                                    \
  auto static constexpr _libproperty__get_metadata(                            \
      decltype(::libproperty::impl::tag_of(name)))                             \
  {                                                                            \
    namespace pi = ::libproperty::impl;                                        \
    return pi::metadata_t<offsetof(host, name), host>{};                       \
  }                                                                            \
  static_assert(true)

namespace libproperty {

template <typename Property, typename Host, typename Tag>
class wrapper {
  // the very first thing to make sure it shares the address with wrapper.
  Property value;

  friend Host;
  friend ::libproperty::impl::backdoor;

  using tag = ::libproperty::meta::type_<Tag>; // tags must be cheap and regular

  using host = Host;
  using self = wrapper;
  using value_type = Property;

  constexpr wrapper() = default;
  constexpr wrapper(wrapper const&) = default;
  constexpr wrapper(wrapper&&) = default;
  ~wrapper() = default;
  constexpr wrapper& operator=(wrapper const&) = default;
  constexpr wrapper& operator=(wrapper&&) = default;

  // val-construction
  constexpr wrapper(value_type&& x) noexcept(
      std::is_nothrow_move_constructible_v<value_type>)
      : value(std::move(x))
  {
  }
  constexpr wrapper(value_type const& x) noexcept(
      std::is_nothrow_copy_constructible_v<value_type>)
      : value(x)
  {
  }

  // metadata getter, for convenience
  static constexpr auto metadata()
  {
    return host::_libproperty__get_metadata(tag{});
  }

public:
  /* setter implementation */
  template <typename X>
  decltype(auto) operator=(X&& val) &
  {
    return value.set(
        ::libproperty::impl::get_host<host>(*this), std::forward<X>(val));
  }
  template <typename X>
  decltype(auto) operator=(X&& val) const &
  {
    return value.set(
        ::libproperty::impl::get_host<host>(*this), std::forward<X>(val));
  }
  template <typename X>
  decltype(auto) operator=(X&& val) &&
  {
    return value.set(::libproperty::impl::get_host<host>(std::move(*this)),
        std::forward<X>(val));
  }

  /* get */
  decltype(auto) get() const &
  {
    return value.get(::libproperty::impl::get_host<host>(*this));
  }
  decltype(auto) get() &
  {
    return value.get(::libproperty::impl::get_host<host>(*this));
  }
  decltype(auto) get() &&
  {
    return value.get(::libproperty::impl::get_host<host>(std::move(*this)));
  }

  /* implicit conversions to get */
  operator decltype(auto)() const &
  {
    return get();
  }
  operator decltype(auto)() &
  {
    return get();
  }
  operator decltype(auto)() &&
  {
    return get();
  }

  /* the return-sensitive conversions to get */
  template <typename U>
  operator U() const
  {
    return value.template convert_to<U>(
        ::libproperty::impl::get_host<host>(*this));
  }

  // operators
  template <typename Y>
  friend bool operator==(wrapper const& x, Y const& y)
  {
    return x.get() == y;
  }
  template <typename X>
  friend bool operator==(X const& x, wrapper const& y)
  {
    return x == y.get();
  }
  friend bool operator==(wrapper const& x, wrapper const& y)
  {
    return x.get() == y.get();
  }

  template <typename Y>
  friend bool operator!=(wrapper const& x, Y const& y)
  {
    return x.get() != y;
  }
  template <typename X>
  friend bool operator!=(X const& x, wrapper const& y)
  {
    return x != y.get();
  }
  friend bool operator!=(wrapper const& x, wrapper const& y)
  {
    return x.get() != y.get();
  }

  template <typename Y>
  friend bool operator<(wrapper const& x, Y const& y)
  {
    return x.get() < y;
  }
  template <typename X>
  friend bool operator<(X const& x, wrapper const& y)
  {
    return x < y.get();
  }
  friend bool operator<(wrapper const& x, wrapper const& y)
  {
    return x.get() < y.get();
  }

  template <typename Y>
  friend bool operator>(wrapper const& x, Y const& y)
  {
    return x.get() > y;
  }
  template <typename X>
  friend bool operator>(X const& x, wrapper const& y)
  {
    return x > y.get();
  }
  friend bool operator>(wrapper const& x, wrapper const& y)
  {
    return x.get() > y.get();
  }

  template <typename Y>
  friend bool operator<=(wrapper const& x, Y const& y)
  {
    return x.get() <= y;
  }
  template <typename X>
  friend bool operator<=(X const& x, wrapper const& y)
  {
    return x <= y.get();
  }
  friend bool operator<=(wrapper const& x, wrapper const& y)
  {
    return x.get() <= y.get();
  }

  template <typename Y>
  friend bool operator>=(wrapper const& x, Y const& y)
  {
    return x.get() >= y;
  }
  template <typename X>
  friend bool operator>=(X const& x, wrapper const& y)
  {
    return x >= y.get();
  }
  friend bool operator>=(wrapper const& x, wrapper const& y)
  {
    return x.get() >= y.get();
  }

  template <typename Y>
  friend bool operator<<(wrapper const& x, Y const& y)
  {
    return x.get() << y;
  }
  template <typename X>
  friend bool operator<<(X const& x, wrapper const& y)
  {
    return x << y.get();
  }
  friend bool operator<<(wrapper const& x, wrapper const& y)
  {
    return x.get() << y.get();
  }

  template <typename Y>
  friend bool operator>>(wrapper const& x, Y const& y)
  {
    return x.get() >> y;
  }
  template <typename X>
  friend bool operator>>(X const& x, wrapper const& y)
  {
    return x >> y.get();
  }
  friend bool operator>>(wrapper const& x, wrapper const& y)
  {
    return x.get() >> y.get();
  }
};

template <typename P, typename H, typename Tag>
struct is_property<wrapper<P, H, Tag>> : std::true_type {
  using tag = ::libproperty::meta::type_<Tag>;
};

} // libproperty

#endif
