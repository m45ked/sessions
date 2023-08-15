#pragma once

#include <cassert>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "core/type_traits/is_optional_v.h"
#include "database/Connection_fwd.h"
#include "database/Query_fwd.h"
#include "database/database_export.h"
#include "sqlite3.h"

namespace Database {

namespace detail {

template <typename ValueT>
auto getFromQuery(sqlite3_stmt *stmt, int idx) -> ValueT;

template <typename ValueT>
inline auto getOptionalFromQuery(sqlite3_stmt *stmt, int idx) -> ValueT {
  if (sqlite3_column_type(stmt, idx) == SQLITE_NULL)
    return std::nullopt;

  return getFromQuery<typename ValueT::value_type>(stmt, idx);
}

} // namespace detail

class DATABASE_EXPORT Query {
public:
  Query(std::string_view sql, Connection &connection);
  virtual ~Query();

  auto execute() -> void;

  template <typename ValueT> auto get(std::string_view fieldName) -> ValueT {
    const auto stmt = getRawStatement();
    const auto idx = getColumnIdxFromStatement(fieldName);
    if constexpr (Core::type_traits::is_optional_v<ValueT>)
      return detail::getOptionalFromQuery<ValueT>(stmt, idx);

    return detail::getFromQuery<ValueT>(stmt, idx);
  }

private:
  auto getRawStatement() const -> sqlite3_stmt *;
  auto getColumnIdxFromStatement(std::string_view fieldName) const -> int;

  class Impl;
  std::unique_ptr<Impl> m_impl;
};

} // namespace Database