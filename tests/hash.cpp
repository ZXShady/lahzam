#include <catch2/catch_test_macros.hpp>
#include <lahzam/hash.hpp>

#include <unordered_map>

struct Point2d {
  int         x, y;
  friend bool operator==(Point2d a, Point2d b) { return a.x == b.x && a.y == b.y; }
};

TEST_CASE("hashing", "[lahzam]")
{
  std::unordered_map<Point2d, int, lahzam::hash> map;
  map[{1, 2}] = 3;
  map[{5, 6}] = 7;
  CHECK(map[{1, 2}] == 3);
  CHECK(map[{5, 6}] == 7);
}
