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
  LIBPROPERTY__DECLARE_TAG(name, host);                                        \
  ::libproperty::wrapper<LIBPROPERTY__PARENTHESIZED_TYPE type,                 \
      host::LIBPROPERTY__TAG_NAME(name)>                                       \
      name;                                                                    \
  static_assert(true, "require semicolon")

namespace libproperty {

template <typename Property, typename Tag>
class wrapper {
  // private implementation
  using host = typename Tag::host_type;
  using self = wrapper;
  using value_type = Property;

  // allow `host` to access self::value
  friend host;
  // the very first thing to make sure it shares the address with wrapper.
  Property value;

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

  /* get */
  template <typename V = value_type,
      typename H = host,
      typename = decltype(std::declval<V>().get(std::declval<H const&>())),
      bool nxc = noexcept(std::declval<V>().get(std::declval<H const&>()))>
  auto get() const & noexcept(nxc) -> decltype(auto)
  {
    return value.get(::libproperty::impl::get_host(*this));
  }
  template <typename V = value_type,
      typename H = host,
      typename = decltype(std::declval<V>().get(std::declval<H&>())),
      bool nxc = noexcept(std::declval<V>().get(std::declval<H&>()))>
      auto get() & noexcept(nxc) -> decltype(auto)
  {
    return value.get(::libproperty::impl::get_host(*this));
  }
  template <typename V = value_type,
      typename H = host,
      typename = decltype(std::declval<V>().get(std::declval<H&&>())),
      bool nxc = noexcept(std::declval<V>().get(std::declval<H&&>()))>
      auto get() && noexcept(nxc) -> decltype(auto)
  {
    return value.get(::libproperty::impl::get_host(std::move(*this)));
  }

public:
  /* setter implementation */
  template <typename X>
  decltype(auto) operator=(X&& val) &
  {
    return value.set(
        ::libproperty::impl::get_host(*this), std::forward<X>(val));
  }
  template <typename X>
  decltype(auto) operator=(X&& val) const &
  {
    return value.set(
        ::libproperty::impl::get_host(*this), std::forward<X>(val));
  }
  template <typename X>
  decltype(auto) operator=(X&& val) &&
  {
    return value.set(
        ::libproperty::impl::get_host(std::move(*this)), std::forward<X>(val));
  }

  /* implicit conversions to get */
  template <typename W = wrapper,
      bool nxc = noexcept(std::declval<W const&>().get())>
  operator decltype(std::declval<W const&>().get())() const & noexcept(nxc)
  {
    return get();
  }
  template <typename W = wrapper, bool nxc = noexcept(std::declval<W&>().get())>
      operator decltype(std::declval<W&>().get())() & noexcept(nxc)
  {
    return get();
  }
  template <typename W = wrapper,
      bool nxc = noexcept(std::declval<W&&>().get())>
  operator decltype(std::declval<W&&>().get())() &&
  {
    return get();
  }

  /* the return-sensitive conversions to get */
  template <typename U,
      typename V = value_type,
      typename H = host,
      bool nxc = noexcept(
          std::declval<V>().template convert_to<U>(std::declval<H const&>()))>
  operator U() const noexcept(nxc)
  {
    return value.template convert_to<U>(::libproperty::impl::get_host(*this));
  }

// operators
#define LIBPROPERTY__DECLARE_OPERATOR(op)                                      \
  template <typename Y>                                                        \
  friend decltype(auto) operator op(wrapper const& x, Y const& y)              \
  {                                                                            \
    return x.get() op y;                                                       \
  }                                                                            \
  template <typename X>                                                        \
  friend decltype(auto) operator op(X const& x, wrapper const& y)              \
  {                                                                            \
    return x op y.get();                                                       \
  }                                                                            \
  friend decltype(auto) operator op(wrapper const& x, wrapper const& y)        \
  {                                                                            \
    return x.get() op y.get();                                                 \
  }                                                                            \
  static_assert("require semicolon")

  LIBPROPERTY__DECLARE_OPERATOR(==);
  LIBPROPERTY__DECLARE_OPERATOR(!=);
  LIBPROPERTY__DECLARE_OPERATOR(<);
  LIBPROPERTY__DECLARE_OPERATOR(>);
  LIBPROPERTY__DECLARE_OPERATOR(<=);
  LIBPROPERTY__DECLARE_OPERATOR(>=);
  LIBPROPERTY__DECLARE_OPERATOR(>>);
  LIBPROPERTY__DECLARE_OPERATOR(<<);
  LIBPROPERTY__DECLARE_OPERATOR(+);
  LIBPROPERTY__DECLARE_OPERATOR(-);
  LIBPROPERTY__DECLARE_OPERATOR(*);
  LIBPROPERTY__DECLARE_OPERATOR(/);
  LIBPROPERTY__DECLARE_OPERATOR(%);
#undef LIBPROPERTY__DECLARE_OPERATOR
};

template <typename P, typename Tag>
struct property_traits<wrapper<P, Tag>> : std::true_type {
  static constexpr std::true_type is_property = {};
  using tag = Tag;
  using meta = void; // wrapper does not use a meta.
};

} // libproperty

#endif
