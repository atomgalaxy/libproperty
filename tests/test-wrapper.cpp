#include "libproperty/wrapper.hpp"

#include <cassert>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

/**
 * An example of how to construct a wrapped property.
 *
 * This example is not optimal - the optimal way of implementing it is to put
 * the first_ and second_ members in the counted<> class, so that we don't waste
 * two bytes (and another however many of alignment) after the two property
 * accessors.
 *
 * It does demonstrate how to access the host's members, however.
 */
template <typename T, typename U>
class counted_pair {
  T first_;
  U second_;

  template <auto member_ptr>
  struct counted {
    static_assert(std::is_member_pointer_v<decltype(member_ptr)>);

    mutable int written = 0;
    mutable int read = 0;
    mutable int converted = 0;

    template <typename Host, typename Value>
    decltype(auto) set(Host&& host, Value&& value) const
    {
      ++written;
      host.*member_ptr = std::forward<Value>(value);
      return std::forward<Host>(host).*member_ptr;
    };

    template <typename Host>
    auto get(Host&& host) const -> T const&
    {
      ++read;
      return std::forward<Host>(host).*member_ptr;
    }

    template <typename Wanted, typename Host>
    auto convert_to(Host&& host) const -> Wanted
    {
      ++converted;
      return static_cast<Wanted>(
          host.*member_ptr); // if a conversion exists, it'll happen.
    }
  };

public:
  LIBPROPERTY_WRAP((counted<&counted_pair::first_>), first, counted_pair);
  LIBPROPERTY_WRAP((counted<&counted_pair::second_>), second, counted_pair);

  auto first_written() const
  {
    return first.value.written;
  }
  auto first_read() const
  {
    return first.value.read;
  }
  auto first_converted() const
  {
    return first.value.converted;
  }
  auto second_written() const
  {
    return second.value.written;
  }
  auto second_read() const
  {
    return second.value.read;
  }
  auto second_converted() const
  {
    return second.value.converted;
  }
};

struct A {
  int x;

  explicit operator int() const
  {
    return x;
  }
};

int main()
{
  {
    counted_pair<int, int> x;
    x.first = 5;
    assert(x.first_written() == 1);
    int const y = x.first;
    assert(x.first_read() == 1);
    assert(x.first == y);
    assert(x.first_read() == 2);
    x.second = x.first;
    assert(x.first_read() == 3);
    assert(x.second_read() == 0);
    assert(x.second_written() == 1);
    assert(x.second == 5);
    assert(x.second_read() == 1);
  }
  {
    counted_pair<std::string, int> x;
    x.first = "foo";
    assert(x.first_written() == 1);
    std::string const y = x.first;
    assert(x.first_read() == 1);
    assert(x.first == y);
    assert(x.first_read() == 2);
  }
  {
    counted_pair<A, std::string> x;
    A y{ 1 };
    x.first = y;
    assert(x.first_written() == 1);
    assert((int)x.first == 1);
    assert(x.first_read() == 0);
    assert(x.first_converted() == 1);
  }
}
