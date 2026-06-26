#pragma once

#include "../lahzam.hpp"
#include <nlohmann/json.hpp>
#include <nlohmann/adl_serializer.hpp>

template<lahzam::reflectable T>
struct nlohmann::adl_serializer<T> {
  static void to_json(json& j, const T& value)
  {
    std::size_t i = 0;
    lahzam::apply(
      [&](const auto&... members) {
        char c[] = {(j[lahzam::member_names<T>[i++]] = members, char())..., char()};
        (void)c;
      },
      value);
  }

  static void from_json(const json& j, T& value)
  {
    std::size_t i = 0;
    lahzam::apply(
      [&](auto&... members) {
        char c[] = {(j.at(lahzam::member_names<T>[i++]).get_to(members), char())..., char()};
        (void)c;
      },
      value);
  }
};
