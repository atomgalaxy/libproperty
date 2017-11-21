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

namespace libproperty {
/**
 * `is_property` trait.
 *
 * Any implemented properties must specialize the is_property trait.
 */
template <typename T>
struct is_property : std::false_type {
};
template <typename T>
inline constexpr is_property<T> is_property_v{};
template <typename T>
using is_property_t = typename is_property<T>::type;

namespace impl {

  /**
   * The bare metadata required to map the member's `this` pointer to the
   * host's `this` pointer.
   */
  template <size_t MemberOffset, typename Host>
  struct metadata {
    using host_type = Host;
    static constexpr auto member_offset = MemberOffset;
  };

  template <size_t MemberOffset, typename Host>
  using metadata_t = metadata<MemberOffset, std::decay_t<Host>>;

  template <size_t MemberOffset, typename Host, auto Getter, auto Setter>
  struct rw_property_metadata : metadata<MemberOffset, Host> {
    static constexpr auto getter = Getter;
    static constexpr auto setter = Setter;
  };

  template <size_t MemberOffset, typename Host, auto Getter, auto Setter>
  using rw_property_metadata_t
      = rw_property_metadata<MemberOffset, std::decay_t<Host>, Getter, Setter>;

  struct backdoor {
    template <typename Property>
    auto static constexpr tag_of = typename std::decay_t<Property>::tag{};
  };

  template <typename Property>
  auto constexpr tag_of(Property&&)
  {
    static_assert(::libproperty::is_property_v<std::decay_t<Property>>);
    return backdoor::tag_of<Property>;
  }

  template <typename Host, typename Property>
  auto constexpr offset_of(Property const& property)
  {
    static_assert(::libproperty::is_property_v<std::decay_t<Property>>);
    return Host::_libproperty__get_metadata(tag_of(property)).member_offset;
  }

  /**
   * @param property should be the '*this' pointer of an rw_property or
   * compatible class.
   */
  template <typename Host, typename Property>
  constexpr auto get_host(Property&& property) noexcept -> decltype(auto)
  {
    namespace pm = ::libproperty::meta;

    static_assert(::libproperty::is_property_v<std::decay_t<Property>>);
    // transfer cv qualifiers from property to host
    using host_ptr_t = std::add_pointer_t<
        pm::like_t<std::remove_reference_t<Property>, Host>>;
    using char_ptr_t = std::add_pointer_t<
        pm::like_t<std::remove_reference_t<Property>, char>>;

    // find the offset and apply it. At runtime, all this code amounts to one
    // adjustment of the 'this' pointer by a constant, so most probably one
    // load and one add.
    auto const property_addr = ::std::addressof(property);
    auto const raw_property_ptr = reinterpret_cast<char_ptr_t>(property_addr);
    auto const offset = offset_of<Host>(property);
    auto const raw_host_ptr = raw_property_ptr - offset;
    auto const host_ptr = reinterpret_cast<host_ptr_t>(raw_host_ptr);
    return pm::forward_like<Property>(*host_ptr);
  }

} // impl
} // property

#endif
