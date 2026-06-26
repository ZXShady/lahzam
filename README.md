# Lahzam

Lahzam is a modern C++20 header-only library for fast and simple compile-time reflection of aggregate types.

API contains very simple functions

1. `lahzam::tie(aggregate)` converts an aggregate to a `std::tuple<Members&...>` of references to apply algorithms on it.
2. `lahzam::member_count<T>` gives the member count of aggregates or -1 on error
3. `lahzam::member_names<T>` an object that iterates over all names of members.
4. `lahzam::get<I>(aggregate)` equalivent to `std::get<I>(lahzam::tie(aggregate))`
5. `lahzam::get<"Name">(aggregate)` gives the member of the specific name.
6. `lahzam::apply(f,aggregate)` similar to `std::tuple` apply. 
7. `lahzam::for_each_member(f,aggregate)` similar to apply except the callable only takes a single arguement
8. `lahzam::for_each_member_with_index(f,aggregate)` similar to `lahzam::for_each_member` except the callable takes the member and an index 
9. `lahzam::for_each_member_with_name(f,aggregate)` similar to `lahzam::for_each_member` except the callable takes the member and its member name 

Examples


```cpp
#include <lahzam/lahzam.hpp>
#include <iostream>

struct Point { int x, y; };

int main() {
    Point p{10,20};
    static_assert(lahzam::member_count<Point> == 2);
    static_assert(lahzam::member_names<Point>[0] == "x");
    static_assert(lahzam::member_names<Point>[1] == "y");
    static_assert(lahzam::member_names<Point>.size() == 2);
    for(auto n : lahzam::member_names<Point>)
    {
        std::cout << n;
    }


    int x,y;
    // Access members by index
    x = lahzam::get<0>(p); // x == 10
    y = lahzam::get<1>(p); // y == 20
    // Access members by name
    x = lahzam::get<"x">(p); // x == 10
    y = lahzam::get<"y">(p); // y == 20

   // Tie into a tuple
   std::tuple<int&,int&> tuple_ref = lahzam::tie(p);

   // Algorithms
   lahzam::apply([](int x,int y)
   {
        std::cout << x << y;
   },p)
   lahzam::for_each_member([](auto& m)
   {
        std::cout << m;
   },p);
   lahzam::for_each_member_with_index([](auto& m,size_t idx)
   {
        std::cout << idx << m;
   },p);
   lahzam::for_each_member_with_name([](auto& m,std::string_view name)
   {
        std::cout << name << m;
   },p);
}
```

## Benchmarks

`MEMBERS` is the amount of members in each type tested.
`TYPES` is the amount of types in each test being tested.

[!NOTE]
Tests were conducted on C++23 (glaze requires it), so no experimental C++26 language reflection was utilized. C++26 Reflective implementations will yield significantly different results.

Compiled using Clang 20.1.2 on Windows 10.


| MEMBERS | TYPES | `boost::pfr::get` | `lahzam::get` | `glz::to_tie` + `get` |
| --- | --- | --- | --- | --- |
| 128 | 32 | 9.48s | **5.55s** | 8.51s |
| 64 | 64 | 6.71s | **4.26s** | 5.75s |
| 32 | 128 | 6.00s | **3.68s** | 4.56s |
| 16 | 256 | 6.43s | **3.74s** | 4.41s |

* `glaze` does not expose directly a `get` function it instead exposes `to_tie` which we can use.

lahzam is consistently ~1.7× faster than Boost and slightly faster than glaze.


| MEMBERS | TYPES | `boost::pfr::get_name` | `lahzam::member_names` | `glz::reflect::keys` |
| --- | --- | --- | --- | --- |
| 128 | 32 | 14.55s | **3.84s** | 21.87s |
| 64 | 64 | 13.23s | **3.69s** | 17.72s |
| 32 | 128 | 13.00s | **4.34s** | 15.97s |
| 16 | 256 | 13.93s | **4.52s** | 15.49s |

Lahzam is up to ~3× faster member name access compared to Boost and ~4× faster than Glaze.

## CMake Integration

```cmake
add_subdirectory(third_party/lahzam)
target_link_libraries(tests PRIVATE lahzam::lahzam)
```

## Limitations

The library does not work if the type contains C arrays

```cpp
struct X 
{
    int arr[4];
};
X x;
lahzam::get<0>(x); // error
```

Getting names of members with types who contain references does not work

```cpp
struct X 
{
    int& ref;
};
X x{var};
lahzam::get<0>(x); // works
lahzam::get<"ref">(x); // error
lahzam::member_names<X>[0]; // error
```

It does not support inheritance yet

```cpp

struct X { int x;};

struct Y : X {int y;};
lahzam::get<0>(Y{}); // error
```