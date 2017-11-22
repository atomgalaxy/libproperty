#ifndef INCLUDED_LIBPROPERTY_PROPERTY_IMPL_HPP
#define INCLUDED_LIBPROPERTY_PROPERTY_IMPL_HPP

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

#include "meta.hpp"
#include <cstddef> // for std::size_t

#define LIBPROPERTY__PARENTHESIZED_TYPE(...) __VA_ARGS__

#define LIBPROPERTY__TAG_NAME(name) _libproperty__##name##_prop_tag

#define LIBPROPERTY__DECLARE_TAG(name, host)                                   \
  struct LIBPROPERTY__TAG_NAME(name) {                                         \
    using host_type = host;                                                    \
    auto static constexpr offset()                                             \
    {                                                                          \
      return std::integral_constant<size_t, offsetof(host, name)>{};           \
    }                                                                          \
  };                                                                           \
  static_assert(true, "need semicolon")

namespace libproperty {
/**
 * `property_traits` trait.
 *
 * Any implemented properties must specialize the property_traits trait.
 */
template <typename T>
struct property_traits {
  static constexpr std::false_type is_property = {};
  using tag = void;
};
template <typename Property>
using property_traits_t = property_traits<std::decay_t<Property>>;

namespace impl {

  template <typename Property>
  using tag_type = typename ::libproperty::property_traits_t<Property>::tag;

  template <typename Property>
  using meta_type = typename ::libproperty::property_traits_t<Property>::meta;

  template <typename Property>
  using host_type = typename tag_type<Property>::host_type;

  template <typename Property>
  auto constexpr tag_of(Property&&)
  {
    static_assert(::libproperty::property_traits_t<Property>::is_property);
    return tag_type<Property>{};
  }

  template <typename Property>
  constexpr auto meta(Property&&)
  {
    return ::libproperty::impl::meta_type<Property>{};
  }

  template <typename Property>
  auto constexpr offset_of(Property&& property)
  {
    return ::libproperty::impl::tag_of(property).offset();
  }

  /**
   * @param property should be the '*this' pointer of an rw_property or
   * compatible class.
   */
  template <typename Property>
  constexpr auto get_host(Property&& property) noexcept -> decltype(auto)
  {
    namespace pm = ::libproperty::meta;

    static_assert(::libproperty::property_traits_t<Property>::is_property);
    // transfer cv qualifiers from property to host
    using host = host_type<Property>;
    using host_ptr_t = std::add_pointer_t<
        pm::like_t<std::remove_reference_t<Property>, host>>;
    using char_ptr_t = std::add_pointer_t<
        pm::like_t<std::remove_reference_t<Property>, char>>;

    // find the offset and apply it. At runtime, all this code amounts to one
    // adjustment of the 'this' pointer by a constant, so most probably one
    // load and one add.
    auto const property_addr = ::std::addressof(property);
    auto const raw_property_ptr = reinterpret_cast<char_ptr_t>(property_addr);
    auto const offset = offset_of(property);
    auto const raw_host_ptr = raw_property_ptr - offset;
    auto const host_ptr = reinterpret_cast<host_ptr_t>(raw_host_ptr);
    return pm::forward_like<Property>(*host_ptr);
  }

} // impl
} // property

#endif
