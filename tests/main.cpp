#include <lahzam/lahzam.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>


namespace MyLib {
struct Person {
  std::string name;
  int         age;
  double      score;
};
} // namespace MyLib

template<typename T, typename U>
struct PairWrapper {
  T               first;
  U               second;
  std::pair<T, U> real;
};

struct Outer {
  struct Inner {
    int   id;
    float value;
  };
  int   id;
  Inner nested;
};

struct ComplexData {
  struct {
    int x;
    int y;
  } point;
  int id;
};

namespace {

template<typename Blah, typename Boo = std::wstring, typename Bruh = Boo>
struct BasicInternalType {
  int code;
};

using InternalType = BasicInternalType<long>;

} // namespace

TEST_CASE("tets", "[lahzam]")
{
  SECTION("tie")
  {
    MyLib::Person p{"Alice", 30, 95.5};
    auto          tied = lahzam::tie(p);

    STATIC_REQUIRE(std::is_same_v<decltype(tied), std::tuple<std::string&, int&, double&>>);

    CHECK(std::get<0>(tied) == "Alice");
    CHECK(std::get<1>(tied) == 30);

    std::get<1>(tied) = 31;
    std::get<0>(tied) = "Bob";

    CHECK(p.age == 31);
    CHECK(p.name == "Bob");

    const MyLib::Person cp{"Const", 100, 0.0};
    auto                c_tied = lahzam::tie(cp);
    STATIC_REQUIRE(std::is_same_v<decltype(c_tied), std::tuple<const std::string&, const int&, const double&>>);
  }

  SECTION("member_count")
  {
    STATIC_REQUIRE(lahzam::member_count<MyLib::Person> == 3);
    STATIC_REQUIRE(lahzam::member_count<PairWrapper<int, float>> == 3);
    STATIC_REQUIRE(lahzam::member_count<Outer> == 2);
    STATIC_REQUIRE(lahzam::member_count<Outer::Inner> == 2);
    STATIC_REQUIRE(lahzam::member_count<ComplexData> == 2);
    STATIC_REQUIRE(lahzam::member_count<InternalType> == 1);
  }

  SECTION("member_names")
  {
    {

      using T = MyLib::Person;
      CHECK(lahzam::member_names<T>[0] == "name");
      CHECK(lahzam::member_names<T>[1] == "age");
      CHECK(lahzam::member_names<T>[2] == "score");
      std::string total = "nameagescore";
      std::string sum;
      for (std::string_view name : lahzam::member_names<T>) {
        sum += name.data(); // null terminated
      }
      CHECK(sum == total);
    }


    {
      using T = PairWrapper<int, double>;
      CHECK(lahzam::member_names<T>[0] == "first");
      CHECK(lahzam::member_names<T>[1] == "second");
      CHECK(lahzam::member_names<T>[2] == "real");

      std::string total = "firstsecondreal";
      std::string sum;
      for (std::string_view name : lahzam::member_names<T>) {
        sum += name.data(); // null terminated
      }
      CHECK(sum == total);
    }
  }

  SECTION("get<Index>")
  {
    Outer obj{100, {1, 5.5f}};

    CHECK(lahzam::get<0>(obj) == 100);

    auto& inner = lahzam::get<1>(obj);
    CHECK(lahzam::get<0>(inner) == 1);
    CHECK(lahzam::get<1>(inner) == 5.5f);

    lahzam::get<0>(obj) = 200;
    CHECK(obj.id == 200);

    const Outer c_obj{100, {1, 5.5f}};
    STATIC_REQUIRE(std::is_same_v<decltype(lahzam::get<0>(c_obj)), const int&>);
  }

  SECTION("get<Name>")
  {
    MyLib::Person p{"Lol", 40, 88.0};

    CHECK(lahzam::get<"name">(p) == "Lol");
    CHECK(lahzam::get<"age">(p) == 40);

    lahzam::get<"score">(p) = 99.9;
    CHECK(p.score == 99.9);

    PairWrapper<std::string, int> pw{"Test", 123, {"Test2",321}};
    CHECK(lahzam::get<"first">(pw) == "Test");
    CHECK(lahzam::get<"real">(pw).first == "Test2");

    ComplexData cd{{10, 20}, 999};
    auto&       pt = lahzam::get<"point">(cd);
    CHECK(pt.x == 10);
    CHECK(lahzam::get<"x">(pt) == 10);
  }

  SECTION("apply")
  {
    MyLib::Person p{"Dave", 20, 50.0};

    auto formatter = [](const std::string& n, int a, double /*s*/) { return n + std::to_string(a); };

    CHECK(lahzam::apply(formatter, p) == "Dave20");

    auto check = [&p](std::string& n, int& a, double& s) {
      CHECK(&n == &p.name);
      CHECK(&a == &p.age);
      CHECK(&s == &p.score);
      return 0x42;
    };

    CHECK(lahzam::apply(check, p) == 0x42);
  }

  SECTION("for_each_member")
  {
    PairWrapper<int, std::string> pw{10, "String", {20, "Strung"}};
    int                           sum = 0;

    lahzam::for_each_member(
      [&](auto&& member) {
        using Type = std::decay_t<decltype(member)>;
        if constexpr (std::is_same_v<Type, int>) {
          sum += static_cast<int>(member);
        }
      },
      pw);

    CHECK(sum == 10);

    lahzam::for_each_member([](auto&& member) { member = {}; }, pw);

    CHECK(pw.first == 0);
    CHECK(pw.second == "");
    CHECK(pw.real.first == 0);
    CHECK(pw.real.second == "");
  }

  SECTION("anon type refl")
  {
    ComplexData c{{5, 10}, 1};
    auto&       point = lahzam::get<"point">(c);

    using AnonType = std::decay_t<decltype(point)>;

    STATIC_REQUIRE(lahzam::member_count<AnonType> == 2);
    CHECK(lahzam::member_names<AnonType>[0] == "x");
    CHECK(lahzam::member_names<AnonType>[1] == "y");
    CHECK(lahzam::get<0>(point) == 5);
    CHECK(lahzam::get<1>(point) == 10);
  }
}