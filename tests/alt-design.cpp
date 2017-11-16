#include "libproperty/wrapper.hpp"

#include <cassert>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

template <typename T>
class X {
  T value;

  struct access_counter {
    mutable int written = 0;
    mutable int read = 0;
    mutable int converted = 0;

    template <typename Host, typename Value>
    decltype(auto) set(Host&& host, Value&& value) const
    {
      ++written;
      host.value = std::forward<Value>(value);
      return std::forward<Host>(host).value;
    };

    template <typename Host>
    auto get(Host&& host) const -> T const&
    {
      ++read;
      return std::forward<Host>(host).value;
    }

    template <typename Wanted, typename Host>
    auto convert_to(Host&& host) const -> Wanted
    {
      ++converted;
      return static_cast<Wanted>(host.value); // if a conversion exists, it'll happen.
    }
  };

public:
  LIBPROPERTY_WRAP((access_counter), counted, X);

  auto written() const { return counted.value.written; }
  auto read() const { return counted.value.read; }
  auto converted() const { return counted.value.converted; }
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
    X<int> x;
    x.counted = 5;
    assert(x.written() == 1);
    int y = x.counted;
    assert(x.read() == 1);
    assert(x.counted == y);
    assert(x.read() == 2);
  }
  {
    X<std::string> x;
    x.counted = "foo";
    assert(x.written() == 1);
    std::string y = x.counted;
    assert(x.read() == 1);
    assert(x.counted == y);
    assert(x.read() == 2);
  }
  {
    X<A> x;
    A y{1};
    x.counted = y;
    assert(x.written() == 1);
    assert((int)x.counted == 1);
    assert(x.read() == 0);
    assert(x.converted() == 1);
  }
}
