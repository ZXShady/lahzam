#include <catch2/catch_test_macros.hpp>
#include <lahzam/addons/nlohmann_json.hpp>

struct Point2d {
  int x, y;
};

TEST_CASE("nlohmann-json integration", "[lahzam]")
{
  nlohmann::json j = Point2d{1, 2};
  CHECK(j["x"].get<int>() == 1);
  CHECK(j["y"].get<int>() == 2);
  const auto point = j.get<Point2d>();
  CHECK(point.x == 1);
  CHECK(point.y == 2);
}
