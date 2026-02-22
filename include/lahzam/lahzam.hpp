#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>
#include <utility>

#ifndef LAHZAM_TUPLE
  #include <tuple>
#endif
namespace lahzam {
namespace details {
#ifndef LAHZAM_TUPLE
  using ::std::tuple;
#else
  LAHZAM_TUPLE;
#endif
} // namespace details
} // namespace lahzam

#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wundefined-var-template"
#endif

namespace lahzam {

namespace details {


  template<std::size_t I, typename T>
  struct tuple_leaf {
    T value;
  };

  template<typename, typename...>
  struct tuple_ref_impl;

  template<std::size_t... Is, typename... Ts>
  struct tuple_ref_impl<std::index_sequence<Is...>, Ts...> : public tuple_leaf<Is, Ts>... {
    template<typename... Us>
    constexpr tuple_ref_impl(Us&... us) noexcept : tuple_leaf<Is, Ts>(us)...
    {
    }
    constexpr tuple_ref_impl(const tuple_ref_impl&)            = default;
    constexpr tuple_ref_impl(tuple_ref_impl&&)                 = default;
    constexpr tuple_ref_impl& operator=(const tuple_ref_impl&) = default;
    constexpr tuple_ref_impl& operator=(tuple_ref_impl&&)      = default;
  };

  template<std::size_t I, typename T>
  constexpr decltype(auto) get(details::tuple_leaf<I, T>& leaf) noexcept
  {
    return leaf.value;
  }
  template<std::size_t I, typename T>
  constexpr decltype(auto) get(const details::tuple_leaf<I, T>& leaf) noexcept
  {
    return leaf.value;
  }

  // template<typename... Ts>
  // using tuple_ref = tuple_ref_impl<std::make_index_sequence<sizeof...(Ts)>, Ts...>;
  template<typename... Ts>
  using tuple_ref_t = tuple_ref_impl<std::make_index_sequence<sizeof...(Ts)>, Ts...>;

  template<typename... Ts >
  struct tuple_ref : tuple_ref_t<Ts...> { using tuple_ref_t<Ts...>::tuple_ref_t;};
  
  template<typename ... Ts>
  tuple_ref(Ts&...) -> tuple_ref<Ts&...>;

  template<typename T>
  concept reflectable = std::is_aggregate_v<T>;

} // namespace details

template<typename T>
concept reflectable = details::reflectable<std::remove_cvref_t<T>>;

namespace details {
  struct any {
    any();
    any(std::size_t);
    template<typename T>
    operator T&() const;
    template<typename T>
    operator T&&() const;
  };

  template<typename, typename>
  bool is_constructible_with;

  template<typename T, std::size_t... Is>
  inline constexpr bool is_constructible_with<T, std::index_sequence<Is...>> = requires { T{any{Is}...}; };


  template<typename T, std::size_t I, std::size_t... Is>
  constexpr std::size_t classic() noexcept
  {
    if constexpr (!details::is_constructible_with<T, std::index_sequence<I, Is...>>) {
      return details::classic<T, Is...>();
    }
    else {
      return sizeof...(Is) + 1;
    }
  }

  template<typename T, std::size_t I>
  constexpr std::size_t classic() noexcept
  {
    if constexpr (requires { T{any{}}; }) {
      return 1;
    }
    else {
      static_assert(sizeof(T) == 0, "What happened?");
    }
  }

  template<typename T, std::size_t Low, std::size_t High>
  constexpr std::size_t bin_search() noexcept
  {
    if constexpr (Low >= High) {
      return Low;
    }
    else {
      constexpr std::size_t Mid = (Low + High + 1) / 2;
      if constexpr (details::is_constructible_with<T, std::make_index_sequence<Mid>>) {
        return details::bin_search<T, Mid, High>();
      }
      else {
        return details::bin_search<T, Low, Mid - 1>();
      }
    }
  }


  template<typename T>
  constexpr auto member_count = []<std::size_t... Is>(std::index_sequence<Is...>) {
    return std::is_empty_v<T> ? 0 : details::classic<T, Is...>();
  }(std::make_index_sequence<256>{});

  template<typename T>
    requires std::default_initializable<T>
  constexpr auto member_count<T> = std::is_empty_v<T> ? 0 : details::bin_search<T, 0, 256>();

}; // namespace details


template<reflectable T>
constexpr std::size_t member_count = details::member_count<std::remove_cvref_t<T>>;


namespace details {

  template<typename Object>
  constexpr decltype(auto) tie(Object& object) noexcept
  {
    // using this weird triple char non descriptive name since the generator will never reach this name
    constexpr auto ZZZ = lahzam::member_count<Object>;
    // Don't try to format this line
    // clang-format off
    if constexpr (ZZZ == 0)
    {
        return lahzam::details::tuple_ref_t<>{};
    } else
    {
    #include "generated.hpp"
    }
    // clang-format on

    //#include "generated.hpp"
  }

  template<template<typename...> class Tuple, typename... Ts, std::size_t... Is>
  constexpr auto tuple_ref_to_tuple(const tuple_ref_t<Ts...>& t, std::index_sequence<Is...>) noexcept
  {
    return Tuple<Ts...>(details::get<Is>(t)...);
  }

} // namespace details

template<template<typename...> class Tuple = lahzam::details::tuple, typename Object>
constexpr decltype(auto) tie(Object& object) noexcept
{
  static_assert(reflectable<Object>);

  return details::tuple_ref_to_tuple<Tuple>(details::tie(object), std::make_index_sequence<member_count<Object>>{});
}
namespace details {

  template<typename T>
  struct wrapper {
    const T                 value;
    static const wrapper<T> faker;
  };

  template<typename T>
  consteval const T& get_faker() noexcept
  {
    return wrapper<T>::faker.value;
  }

  // whole purpose is to add a comma
  inline constexpr int comma_man = 0;
  struct Members {
    char        names[4096]{};
    std::size_t lengths[384]{};
    std::size_t total_length{};
  };


#define SZC(x) (sizeof(x) - 1)
#if defined(__clang__)

  template<auto&... V>
  constexpr auto name_of() noexcept
  {
    return __PRETTY_FUNCTION__ + SZC("auto lahzam::details::name_of() [V = <");
  }

  template<typename T, std::size_t... Is>
  constexpr auto name(std::index_sequence<Is...>) noexcept
  {
    constexpr auto t = lahzam::details::tie(details::get_faker<T>());
    constexpr auto n = details::name_of<details::get<Is>(t)..., comma_man>();
    return n;
  }

  // auto __cdecl lahzam::details::name_of<struct Pair,& lahzam::details::faker<struct Pair>->x,& lahzam::details::faker<struct Pair>->y,& lahzam::details::faker<struct Pair>->z>(void) noexcept
  constexpr void parse_members(const char*       str,
                               const bool        null_terminated,
                               char*             buffer,
                               std::size_t*      lengths,
                               const std::size_t count,
                               std::size_t&      max_length)
  {
    const auto buffer_start_copy = buffer;
    for (std::size_t i = 0; i < count; ++i) {
      str += SZC("faker.value.");
      const auto start = buffer;
      while (*str != ',')
        *buffer++ = *str++;
      str += SZC(", ");
      buffer += null_terminated;
      *lengths++ = std::size_t(buffer - start);
    }
    max_length = std::size_t(buffer - buffer_start_copy);
  }


  template<typename T>
  constexpr Members reflection_member_names_get()
  {
    constexpr auto name = details::name<T>(std::make_index_sequence<lahzam::member_count<T>>());

    Members members{};
    details::parse_members(name, true, members.names, members.lengths, lahzam::member_count<T>, members.total_length);
    return members;
  }

#elif defined(__GNUC__)

  template<typename T>
  constexpr auto raw_name_length() noexcept
  {
    //std::cout << "RAWNAME" << __PRETTY_FUNCTION__ << '\n';

    return SZC(__PRETTY_FUNCTION__) - SZC("constexpr auto lahzam::details::raw_name_length() [with T = ]");
  }


//  // IMPORTANT THIS IS VARIAIDC
//  template<typename... T>
//  constexpr auto raw_name_length_long() noexcept
//  {
//    std::cout << "RAWNAME LONG" << __PRETTY_FUNCTION__ << '\n';
//    return SZC(__PRETTY_FUNCTION__) - SZC("constexpr auto lahzam::details::raw_name_length_long() [with T = {]");
//  }

  template<auto&... V>
  constexpr auto name_of() noexcept
  {
    //if (!__builtin_is_constant_evaluated())
    //std::cout << "NAMEOF= "<< __PRETTY_FUNCTION__ << '\n';
    return __PRETTY_FUNCTION__ + SZC("constexpr auto lahzam::details::name_of() [with auto& ...V = {");
  }


  //template<auto&... V>
  //constexpr auto get_member_access_length_func() noexcept
  //{
  //  return std::string_view(__PRETTY_FUNCTION__ +
  //                            SZC("constexpr auto lahzam::details::get_member_access_length_func() [with auto& ...V = "
  //                                "{"),
  //                          SZC(__PRETTY_FUNCTION__) -
  //                            SZC("constexpr auto lahzam::details::get_member_access_length_func() [with auto& ...V = "
  //                                "{}]"));
  //}

  template<typename T, std::size_t... Is>
  constexpr auto name(std::index_sequence<Is...>) noexcept
  {
    constexpr auto t = lahzam::details::tie(details::get_faker<T>());
    return details::name_of<details::get<Is>(t)..., comma_man>();
  }

  // auto __cdecl lahzam::details::name_of<struct Pair,& lahzam::details::faker<struct Pair>->x,& lahzam::details::faker<struct Pair>->y,& lahzam::details::faker<struct Pair>->z>(void) noexcept
  constexpr void parse_members(
    const char* str,
    //const std::size_t class_length,
    const std::size_t member_access_length,
    const bool        null_terminated,
    char*             buffer,
    std::size_t*      lengths,
    const std::size_t count,
    std::size_t&      max_length)
  {
    const auto buffer_start_copy = buffer;
    for (std::size_t i = 0; i < count; ++i) {
      //std::cout << str << '\n';
      //std::cout << class_length << '\n';
      //std::cout << member_access_length << '\n';
      str += SZC("wrapper<") + member_access_length;
      //std::cout << str << '\n';
      if (*str != '.') {
        while (*str != '>') {
          ++str;
        }
        ++str; // skip '>'
      }
      //std::cout << str << '\n';
      str += SZC("::faker.wrapper<") + member_access_length;
      //std::cout << str << '\n';
      if (*str != '.') {
        while (*str != '>') {
          str++;
        }
        ++str; // skip '>'
      }
      //std::cout << str << '\n';
      str += SZC("::value.") + member_access_length + SZC("::");
      while (!((*str >= 'a' && *str <= 'z') || (*str >= 'A' && *str <= 'Z') || *str == '_' || (*str >= '0' && *str <= '9'))) {
        ++str;
      }
      //std::cout << str << '\n';
      const auto start = buffer;
      while (*str != ',')
        *buffer++ = *str++;
      //std::cout << str << '\n';
      str += SZC(", ");
      //std::cout << str << '\n';
      buffer += null_terminated;
      *lengths++ = std::size_t(buffer - start);
    }
    max_length = std::size_t(buffer - buffer_start_copy);
  }


  template<typename T>
  constexpr Members reflection_member_names_get()
  {
    constexpr auto name = details::name<T>(std::make_index_sequence<lahzam::member_count<T>>());

    Members members{};
    details::parse_members(name,
                           //details::raw_name_length_long<T>(),
                           details::raw_name_length<T>(),
                           true,
                           members.names,
                           members.lengths,
                           lahzam::member_count<T>,
                           members.total_length);
    return members;
  }

#elif defined(_MSC_VER)

  template<typename T, auto*... V>
  consteval auto __cdecl name_of() noexcept
  {
  #define SZC(x) (sizeof(x) - 1)
    return __FUNCSIG__ + SZC("auto __cdecl lahzam::details::name_of<");
    //return __FUNCSIG__ + SZC("auto __cdecl lahzam::details::name_of<");
  }


  template<typename T, std::size_t... Is>
  consteval auto name(std::index_sequence<Is...>) noexcept
  {
    constexpr auto t = lahzam::details::tie(details::get_faker<T>());
    constexpr auto n = details::name_of<T, &details::get<Is>(t)..., &comma_man>();
    return n;
  }

  // auto __cdecl lahzam::details::name_of<struct Pair,& lahzam::details::faker<struct Pair>->x,& lahzam::details::faker<struct Pair>->y,& lahzam::details::faker<struct Pair>->z>(void) noexcept
  consteval void parse_members(
    const char*       str,
    const std::size_t class_length,
    const bool        null_terminated,
    char*             buffer,
    std::size_t*      lengths,
    const std::size_t count,
    std::size_t&      max_length)
  {
    const auto buffer_start_copy = buffer;
    str += class_length + SZC(",");
    for (std::size_t i = 0; i < count; ++i) {
      str += SZC("&faker->value->");
      const auto start = buffer;
      while (*str != ',')
        *buffer++ = *str++;
      ++str; // skip comma
      buffer += null_terminated;
      *lengths++ = std::size_t(buffer - start);
    }
    max_length = std::size_t(buffer - buffer_start_copy);
  }

  template<typename T>
  consteval auto __cdecl raw_name_length() noexcept
  {
    return SZC(__FUNCSIG__) - SZC("auto __cdecl lahzam::details::raw_name_length<>(void) noexcept");
  }

  template<typename T>
  consteval Members reflection_member_names_get()
  {
    constexpr auto name = details::name<T>(std::make_index_sequence<lahzam::member_count<T>>());

    Members members{};
    details::parse_members(name,
                           details::raw_name_length<T>(),
                           true,
                           members.names,
                           members.lengths,
                           lahzam::member_count<T>,
                           members.total_length);
    return members;
  }

#endif
#undef SZC

  template<typename T>
  constexpr auto reflection_member_names_storage = []() {
    constexpr Members                      members = details::reflection_member_names_get<T>();
    std::array<char, members.total_length> ret{};
    for (std::size_t i = 0; i < members.total_length; ++i)
      ret[i] = members.names[i];
    return ret;
  }();

  template<typename T>
  constexpr auto reflection_member_names_storage_indices = []() {
    constexpr Members members = details::reflection_member_names_get<T>();
    constexpr auto    Count   = member_count<T>;
    using Length              = std::conditional_t<(members.total_length <= UINT8_MAX), std::uint8_t, std::uint16_t>;
    std::array<Length, Count + 1> ret{};
    Length                        string_index = 0;
    for (std::size_t i = 0; i < Count; ++i) {
      ret[i] = string_index;
      string_index += static_cast<Length>(members.lengths[i]);
    }
    ret[Count] = string_index;
    return ret;
  }();


  struct senitiel {};

  template<typename CRTP, std::ptrdiff_t Size>
  struct sized_iterator {
    static_assert(Size < INT16_MAX, "Too many enum entries");
  private:
    using IndexType = std::conditional_t<(Size <= INT8_MAX), std::int8_t, std::int16_t>;
  public:
    IndexType       index{};
    constexpr CRTP& operator+=(const std::ptrdiff_t offset) & noexcept
    {
      index += static_cast<IndexType>(offset);
      return static_cast<CRTP&>(*this);
    }
    constexpr CRTP& operator-=(const std::ptrdiff_t offset) & noexcept
    {
      index -= static_cast<IndexType>(offset);
      return static_cast<CRTP&>(*this);
    }

    constexpr CRTP& operator++() & noexcept
    {
      ++index;
      return static_cast<CRTP&>(*this);
    }
    constexpr CRTP& operator--() & noexcept
    {
      --index;
      return static_cast<CRTP&>(*this);
    }

    [[nodiscard]] constexpr CRTP operator++(int) & noexcept
    {
      auto copy = static_cast<CRTP&>(*this);
      ++*this;
      return copy;
    }
    [[nodiscard]] constexpr CRTP operator--(int) & noexcept
    {
      auto copy = static_cast<CRTP&>(*this);
      --*this;
      return copy;
    }

    [[nodiscard]] constexpr friend CRTP operator+(CRTP it, const std::ptrdiff_t offset) noexcept
    {
      it += offset;
      return it;
    }

    [[nodiscard]] constexpr friend CRTP operator+(const std::ptrdiff_t offset, CRTP it) noexcept
    {
      it += offset;
      return it;
    }

    [[nodiscard]] constexpr friend CRTP operator-(CRTP it, const std::ptrdiff_t offset) noexcept
    {
      it -= offset;
      return it;
    }

    [[nodiscard]] constexpr std::ptrdiff_t operator-(const sized_iterator that) const noexcept
    {
      return index - that.index;
    }

    [[nodiscard]] constexpr std::ptrdiff_t        operator-(senitiel) const noexcept { return index - Size; }
    [[nodiscard]] friend constexpr std::ptrdiff_t operator-(senitiel, sized_iterator it) noexcept
    {
      return Size - it.index;
    }

    [[nodiscard]] constexpr bool operator==(const sized_iterator that) const noexcept { return that.index == index; };
    [[nodiscard]] constexpr bool operator==(senitiel) const noexcept { return Size == index; }

#ifdef __cpp_impl_three_way_comparison
    [[nodiscard]] constexpr auto operator<=>(const sized_iterator that) const noexcept { return index <=> that.index; };
    [[nodiscard]] constexpr auto operator<=>(senitiel) const noexcept { return index <=> Size; }
#else

    [[nodiscard]] constexpr bool operator!=(const sized_iterator that) const noexcept { return that.index != index; };
    [[nodiscard]] constexpr bool operator!=(senitiel) const noexcept { return Size != index; }

    [[nodiscard]] friend constexpr bool operator==(senitiel, const sized_iterator it) noexcept
    {
      return Size == it.index;
    }


    [[nodiscard]] friend constexpr bool operator!=(senitiel, const sized_iterator it) noexcept
    {
      return Size != it.index;
    }


    [[nodiscard]] constexpr bool operator<(const sized_iterator that) const noexcept { return index < that.index; };
    [[nodiscard]] constexpr bool operator>(const sized_iterator that) const noexcept { return index > that.index; };
    [[nodiscard]] constexpr bool operator<=(const sized_iterator that) const noexcept { return index <= that.index; };
    [[nodiscard]] constexpr bool operator>=(const sized_iterator that) const noexcept { return index >= that.index; };

    [[nodiscard]] constexpr bool operator<(senitiel) const noexcept { return index < Size; };
    [[nodiscard]] constexpr bool operator>(senitiel) const noexcept { return index > Size; };
    [[nodiscard]] constexpr bool operator<=(senitiel) const noexcept { return index <= Size; };
    [[nodiscard]] constexpr bool operator>=(senitiel) const noexcept { return index >= Size; };

    [[nodiscard]] friend constexpr bool operator<(senitiel, const sized_iterator it) noexcept
    {
      return Size < it.index;
    };
    [[nodiscard]] friend constexpr bool operator>(senitiel, const sized_iterator it) noexcept
    {
      return Size > it.index;
    };
    [[nodiscard]] friend constexpr bool operator<=(senitiel, const sized_iterator it) noexcept
    {
      return Size <= it.index;
    };
    [[nodiscard]] friend constexpr bool operator>=(senitiel, const sized_iterator it) noexcept
    {
      return Size >= it.index;
    };

#endif
  };

  template<typename T, typename String = std::string_view>
  struct member_names_t {
    [[nodiscard]] static constexpr std::size_t size() noexcept { return member_count<T>; }

    struct iterator : sized_iterator<iterator, static_cast<std::ptrdiff_t>(size())> {
      using value_type = String;
      [[nodiscard]] constexpr String operator*() const noexcept
      {
        const auto* const strings = details::reflection_member_names_storage<T>.data();
        const auto* const p       = details::reflection_member_names_storage_indices<T>.data();
        return String(strings + p[this->index], p[this->index + 1] - p[this->index] - 1 /*Nullterminated*/);
      }

      [[nodiscard]] constexpr String operator[](const std::ptrdiff_t i) const noexcept { return *(*this + i); }
    };

    [[nodiscard]] static constexpr auto begin() { return iterator{}; }
    [[nodiscard]] static constexpr auto end() { return senitiel{}; }

    [[nodiscard]] constexpr auto operator[](const std::size_t i) const noexcept
    {
      return *(begin() + static_cast<std::ptrdiff_t>(i));
    }
  };


  template<std::size_t N>
  struct fixed_string {
    constexpr fixed_string(const char* const s) noexcept
    {
      for (std::size_t i = 0; i < N; ++i)
        str[i] = s[i];
    }
    enum {
      size = N
    };
    char str[N + 1]{};
  };

  template<std::size_t N>
  fixed_string(const char (&)[N]) -> fixed_string<N - 1>;

  constexpr std::size_t name_to_index(const std::string_view name, const char* names, const std::size_t member_count)
  {
    for (std::size_t m = 0; m < member_count; ++m) {
      if (name == names)
        return m;
      while (*names != '\0')
        ++names;
      ++names; // skip '\0'
    }
    throw "Failed to find match name";
  }

} // namespace details


template<typename T>
inline constexpr details::member_names_t<T> member_names{};

template<std::size_t I, typename Object>
[[nodiscard]] constexpr decltype(auto) get(Object& object) noexcept
{
  static_assert(reflectable<Object>);
  return details::get<I>(lahzam::details::tie(object));
}

template<details::fixed_string S, typename Object>
[[nodiscard]] constexpr decltype(auto) get(Object& object) noexcept
{
  static_assert(reflectable<Object>);
  using O = std::remove_cvref_t<Object>;
  return details::get<details::name_to_index({S.str, S.str + S.size}, lahzam::member_names<O>[0].data(), lahzam::member_count<O>)>(
    lahzam::details::tie(object));
}

namespace details {
  template<typename F, typename Object, std::size_t... Is>
  constexpr decltype(auto) apply(F&& f, Object& object, std::index_sequence<Is...>)
  {
    auto t = details::tie(object);
    return static_cast<F&&>(f)(details::get<Is>(t)...);
  }

} // namespace details

template<typename F, typename Object>
constexpr decltype(auto) apply(F&& f, Object& object)
{
  static_assert(reflectable<Object>);

  return details::apply(static_cast<F&&>(f), object, std::make_index_sequence<member_count<Object>>{});
}

template<typename F, typename Object>
constexpr void for_each_member(F f, Object& object)
{
  static_assert(reflectable<Object>);

  return details::apply(
    [&f](auto&... objects) {
      const char c[] = {((void)f(objects), '\0')...,0};
      (void)c;
    },
    object,
    std::make_index_sequence<member_count<Object>>{});
}

} // namespace lahzam

#if defined(__clang__)
  #pragma clang diagnostic pop
#endif