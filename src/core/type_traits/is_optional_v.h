#pragma once
#include <optional>
#include <type_traits>

namespace Core::type_traits {

template <typename ValueT> struct is_optional : std::false_type {};

template <typename ValueT>
struct is_optional<std::optional<ValueT>> : std::true_type {
  typedef ValueT value_type;
};

template <class T> inline constexpr bool is_optional_v = is_optional<T>::value;

} // namespace Core::type_traits