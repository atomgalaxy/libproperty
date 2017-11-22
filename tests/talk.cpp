#include <libproperty/property.hpp>

#include <optional>
#include <string>
#include <utility>

using Payload = std::string;
using MaybePayload = std::optional<Payload>;

class NoPayloadError {
};

class Airplane {
  MaybePayload hold;

  struct Cargo {
    auto get(Airplane& host) const -> decltype(auto)
    {
      if (!host.hold) {
        throw NoPayloadError{};
      }
      return *std::exchange(host.hold, {});
    }
    MaybePayload set(Airplane& host, Payload payload) const
    {
      if (host.hold) {
        return { std::move(payload) };
      }
      host.hold = std::move(payload);
      return {};
    }
  };

public:
  LIBPROPERTY_WRAP((Cargo), cargo, Airplane);
};

int main()
{
  Airplane airplane;
  airplane.cargo = std::string{ "cargo" };
  std::string x = airplane.cargo;
}
