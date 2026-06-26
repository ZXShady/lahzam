#pragma once

#include "lahzam.hpp"
#include <system_error> // for std::hash

namespace lahzam {
namespace details {
  constexpr void hash_combine(std::size_t& seed, std::size_t value) noexcept
  {
    if constexpr (sizeof(std::size_t) == 8)
      seed = seed ^ (value + 0x9e3779b97f4a7c15ull + (seed << 6) + (seed >> 2));
    else
      seed = seed ^ (value + 0x9e3779b9u + (seed << 6) + (seed >> 2));
  }

  template<typename T>
  constexpr std::size_t hash_value(const T& value);
} // namespace details
struct hash {
  template<typename Object>
  constexpr std::size_t operator()(const Object& object) const noexcept
  {
    std::size_t seed = 0;
    lahzam::apply(
      [&seed](const auto&... members) {
        const char c[] = {(details::hash_combine(seed, details::hash_value(members)), char())..., char()};
        (void)c;
      },
      object);
    return seed;
  }
};


namespace details {

  template<typename T>
  constexpr std::size_t hash_value(const T& value)
  {
    if constexpr (std::is_default_constructible_v<std::hash<T>>) {
      return std::hash<T>{}(value);
    }
    else {
      static_assert(lahzam::is_reflectable<T>);
      return lahzam::hash{}(value);
    }
  }


} // namespace details

} // namespace lahzam