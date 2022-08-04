#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

#include "database/Connection_fwd.h"
#include "database/Query_fwd.h"

namespace Database {

class Query {
public:
  Query(const std::string_view &sql, Connection &connection);
  void execute();
  template <typename ValueT>
  inline ValueT get(const std::string_view &fieldName) {
    if constexpr (std::is_integral_v<ValueT>)
      return getInteger(fieldName);
    else if constexpr (std::is_floating_point_v<ValueT>)
      return getDouble(fieldName);
    else if constexpr (std::is_same_v<ValueT, std::string>)
      return getString(fieldName);

    assert(std::is_trivially_constructible_v<ValueT>);
    return ValueT{};
  }

private:
  double getDouble(const std::string_view &field);
  int64_t getInteger(const std::string_view &field);
  std::string getString(const std::string_view &field);

  class Impl;
  friend class Impl;
  std::shared_ptr<Impl> m_impl;
};

} // namespace Database