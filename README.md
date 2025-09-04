# Lahzam

Lahzam is a modern C++20 header-only library for compile-time reflection of aggregate types. It enables fast and compact access to the members and their names of aggregates types, all without macros, annotations, or boilerplate.


[Benchmarks](#benchmarks)

API contains very simple functions

1. `lahzam::tie(aggregate)` converts an aggregate to a `std::tuple` reference to apply algorithms on it.
2. `lahzam::member_count<T>` gives the member count of aggregates
3. `lahzam::member_names<T>` an object that iterates over all names of members.
4. `lahzam::get<I>(aggregate)` equalivent to `std::get<I>(lahzam::tie(aggregate))`
5. `lahzam::get<"Name">(aggregate)` gives the member of the specific name.
6. `lahzam::apply(f,aggregate)` similar to `std::tuple` apply. 
7. `lahzam::for_each_member(f,aggregate)` similar to apply except the callable only takes a single arguement


Simple Examples

Access members by index

```cpp
#include <lahzam/lahzam.hpp>
struct Point { int x, y; };

int main() {
    Point p{10,20};
    int x = lahzam::get<0>(p); // x == 10
    int y = lahzam::get<1>(p); // y == 20
}
```

Access members by names.

```cpp
#include <lahzam/lahzam.hpp>
struct Point { int x, y; };

int main() {
    Point p{10,20};
    int x = lahzam::get<"x">(p); // x == 10
    int y = lahzam::get<"y">(p); // y == 20
}
```

Tie it into a tuple

```cpp
#include <lahzam/lahzam.hpp>
struct Point { int x, y; };

int main() {
    Point p{10,20};
    std::tuple<int&,int&> tuple_ref = lahzam::tie(p); 
}
```


## Benchmarks

`MEMBERS` is the amount of members in each type tested.
`TYPES` is the amount of types in each test being tested.

Time is in **seconds**.
Compiled using Clang.


| MEMBERS | TYPES | `boost::pfr::get` | `lahzam::get` | `glz::to_tie` + `get` |
|---------|-------|-----------|------------|-----------|
|   128   |   32  |    8.38   |  **4.76**  |    8.26   |
|    64   |   64  |    6.25   |  **3.87**  |    5.54   |
|    32   |  128  |    5.67   |  **3.44**  |    4.66   |
|    16   |  256  |    5.99   |  **3.74**  |    3.96   |

`glaze` does not expose directly a `get` function it instead exposes `to_tie` which we can use.

lahzam is consistently ~2× faster than Boost and slightly faster than glaze.


| MEMBERS | TYPES | `boost::pfr::get_name` | `lahzam::member_names` | `glz::reflect::keys` |
|---------|-------|------------|-------------|----------------|
|   128   |   32  |    13.28   |   **4.21**  |      20.39     |
|    64   |   64  |    12.08   |   **4.28**  |      16.15     |
|    32   |  128  |    11.78   |   **4.60**  |      14.56     |
|    16   |  256  |    12.38   |   **5.59**  |      14.34     |

Lahzam is up to ~3× faster member name access compared to Boost and ~4× faster than Glaze.