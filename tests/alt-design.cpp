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
  };


public:
  LIBPROPERTY_WRAP((access_counter), counted, X);
};



int main()
{
  {
    X<int> x;
    x.counted = 5;
    int y = x.counted;
    assert(x.counted == y);
  }
  {
    X<std::string> x;
    x.counted = "foo";
    std::string y = x.counted;
    assert(x.counted == y);
  }
}
