#ifndef LIBPROPERTY_META_HPP_INCLUDED
#define LIBPROPERTY_META_HPP_INCLUDED

#include <type_traits>
#include <utility>

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

The original idea for this: Louis Dionne, the boost::hana library.
*/

namespace libproperty {
namespace meta {

  template <typename T>
  struct type_ {
    using type = T;
  };

  template <typename T>
  type_<T> type_c;

  template <auto Value>
  struct value_ {
    using type = decltype(Value);
    static constexpr type value{ Value };
    constexpr operator type() const { return Value; };
    static constexpr type get() { return Value; }
  };

  template <typename Type, Type Value>
  value_<Value> value_c;

  template <typename From, typename To>
  class like {
    template <bool Condition, template <typename> class Function, typename T>
    using apply_if = std::conditional_t<Condition, Function<T>, T>;
    using base = std::remove_cv_t<std::remove_reference_t<To>>;
    using base_from = std::remove_reference_t<From>;

    static constexpr bool rv = std::is_rvalue_reference_v<From>;
    static constexpr bool lv = std::is_lvalue_reference_v<From>;
    static constexpr bool c = std::is_const_v<base_from>;
    static constexpr bool v = std::is_volatile_v<base_from>;

  public:
    // clang-format off
    using type = apply_if<lv, std::add_lvalue_reference_t,
                 apply_if<rv, std::add_rvalue_reference_t,
                 apply_if<c, std::add_const_t,
                 apply_if<v, std::add_volatile_t,
                 base>>>>;
    // clang-format on
  };

  template <typename From, typename To>
  using like_t = typename like<From, To>::type;

  template <typename Like, typename T>
  constexpr decltype(auto) forward_like(T&& t) noexcept
  {
    return std::forward<like_t<Like, T>>(std::forward<T>(t));
  }

} // meta
}

#endif
