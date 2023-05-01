#pragma once

#include <cassert>
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <sys/_types/_int64_t.h>
#include <type_traits>
#include <vector>

#include "database/Connection_fwd.h"
#include "database/Query_fwd.h"
#include "database/database_export.h"

namespace Database {

class DATABASE_EXPORT Query {
public:
  Query(const std::string_view &sql, Connection &connection);
  auto execute() -> void;

  template <typename ValueT> ValueT get(const std::string_view &fieldName) {
    if constexpr (std::is_integral_v<ValueT>)
      return getInteger(fieldName);
    else if constexpr (std::is_floating_point_v<ValueT>)
      return getDouble(fieldName);
    else if constexpr (std::is_same_v<ValueT, std::string>)
      return getString(fieldName);
    else if constexpr (std::is_assignable_v<ValueT, std::vector<std::byte> >)
      return getBlob(fieldName);

    assert(std::is_trivially_constructible_v<ValueT>);
    return {};
  }

private:
  auto getDouble(const std::string_view &field) -> double;
  auto getInteger(const std::string_view &field) -> int64_t;
  auto getString(const std::string_view &field) -> std::string;
  auto getBlob(const std::string_view &field) -> std::vector<std::byte>;

  class Impl;
  friend class Impl;
  std::shared_ptr<Impl> m_impl;
};

} // namespace Database