#include "libproperty/property.hpp"

#include <memory>
#include <string>
#include <utility>

class X {
  auto const& set_str(std::string x)
  {
    if (str.value) {
      *str.value = std::move(x);
    } else {
      str.value = std::make_unique<std::string>(std::move(x));
    }
    return *str.value;
  }

  auto const& get_str() const
  {
    static std::string empty{};
    return (str.value) ? *str.value : empty;
  }

public:
  LIBPROPERTY_PROPERTY(
      (std::unique_ptr<std::string>), str, get_str, set_str, X);
};

int main()
{
  {
    X x;
    X y = std::move(x);
  }
  {
    X x;
    x.str = "foo";
    auto y = std::move(x);
  }
  {
#ifdef ENSURE_DOES_NOT_COMPILE_1
    X x;
    X y = x; // no copy
#endif
  }
}
