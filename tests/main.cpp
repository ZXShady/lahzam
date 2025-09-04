#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <lahzam/lahzam.hpp>
#include <string>
#include <tuple>

struct Vertex {
  int              x, y, z;
  float            texX, texY;
  std::string_view name;
};

TEST_CASE("basic tests")
{
  Vertex v{1, 2, 3, 0.5f, 0.1f, "hello"};
  lahzam::get<"x">(v) = 5;
  CHECK(v.x == 5);

  lahzam::get<"texY">(v) = 4.0f;
  CHECK(v.texY == 4.0f);

  constexpr auto names = lahzam::member_names<Vertex>;
  CHECK(names[0] == "x");
  CHECK(names[1] == "y");
  CHECK(names[2] == "z");
  CHECK(names[3] == "texX");
  CHECK(names[4] == "texY");
  CHECK(names[5] == "name");


  static_assert(lahzam::member_count<Vertex> == 6);

  const auto [x, y, z, texX, texY, name] = lahzam::tie(v);
  x                                      = -1;
  y                                      = -2;
  z                                      = -3;
  texX                                   = -4.0f;
  texY                                   = -5.0f;
  name                                   = "Vertex";

  CHECK(x == -1);
  CHECK(y == -2);
  CHECK(z == -3);
  CHECK(texX == -4.0f);
  CHECK(texY == -5.0f);
  CHECK(name == "Vertex");
}

TEST_CASE("lahzam::apply")
{
  Vertex v{1, 2, 3, 0.5f, 0.1f, "hello"};

  lahzam::apply(
    [](auto&... members) {
      ((members =
          [&]() {
            if constexpr (std::is_same_v<std::remove_cvref_t<decltype(members)>, std::string_view>)
              return "world";
            else
              return members + 1;
          }()),
       ...);
    },
    v);

  CHECK(v.x == 2);
  CHECK(v.y == 3);
  CHECK(v.z == 4);
  CHECK(v.texX == Catch::Approx(1.5f));
  CHECK(v.texY == Catch::Approx(1.1f));
  CHECK(v.name == "world");
}

TEST_CASE("lahzam::for_each_member")
{
  Vertex v{1, 2, 3, 0.5f, 0.1f, "hello"};

  int              sum_ints   = 0;
  float            sum_floats = 0.0f;
  std::string_view name;

  lahzam::for_each_member(
    [&](auto& member) {
      using T = std::remove_cvref_t<decltype(member)>;

      if constexpr (std::is_integral_v<T>) {
        sum_ints += member;
      }
      else if constexpr (std::is_floating_point_v<T>) {
        sum_floats += member;
      }
      else if constexpr (std::is_same_v<T, std::string_view>) {
        name = member;
      }
    },
    v);

  CHECK(sum_ints == 6);
  CHECK(sum_floats == Catch::Approx(0.6f));
  CHECK(name == "hello");
}
