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
/* is_property trait.
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
  template <typename MemberPtr, typename Host>
  struct bare_metadata {
    using host_type = Host;

    using member_ptr_t = MemberPtr;
    static constexpr auto member_ptr = member_ptr_t::value;
  };

  template <typename MemberPtr, typename Getter, typename Setter, typename Host>
  struct metadata : bare_metadata<MemberPtr, Host> {

    using getter_t = Getter;
    static constexpr auto getter = Getter::value;

    using setter_type = Setter;
    static constexpr auto setter = Setter::value;
  };

  template <typename MemberPtr, typename Host>
  using bare_metadata_t
      = bare_metadata<std::decay_t<MemberPtr>, std::decay_t<Host>>;

  template <typename MemberPtr, typename Getter, typename Setter, typename Host>
  using metadata_t = metadata<std::decay_t<MemberPtr>,
      std::decay_t<Getter>,
      std::decay_t<Setter>,
      std::decay_t<Host>>;

  template <typename Property>
  auto constexpr tag_of(Property&&)
  {
    namespace pm = ::libproperty::meta;

    static_assert(::libproperty::is_property_v<std::decay_t<Property>>);
    using decayed = std::decay_t<Property>;
    return typename decayed::tag{};
  }

  /**
   * Alternative implementation of offsetof, but with a member pointer instead
   * of a name.
   *
   * At this point, the host type is fully defined, so we can do this magic.
   *
   * Contrary to popular belief, this is *not* undefined behaviour. It is the
   * initial definition of the offsetof() macro.
   */
  template <typename Host, typename PointerToMemberType>
  auto constexpr offset_of(PointerToMemberType member_ptr)
  {
    return reinterpret_cast<std::size_t>(&((Host*)0->*member_ptr));
  }

  template <typename Host, typename Property>
  auto constexpr offset_of_property(Property const& property)
  {
    static_assert(::libproperty::is_property_v<std::decay_t<Property>>);

    auto const tag = tag_of(property);
    auto const member_ptr =
      Host::_libproperty__get_metadata(tag).member_ptr;
    return offset_of<Host>(member_ptr);
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
    auto const offset = offset_of_property<Host>(property);
    auto const raw_host_ptr = raw_property_ptr - offset;
    auto const host_ptr = reinterpret_cast<host_ptr_t>(raw_host_ptr);
    return pm::forward_like<Property>(*host_ptr);
  }

} // impl
} // property

#endif
