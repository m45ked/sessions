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

#include "database/Connection_fwd.h"
#include "database/Query_fwd.h"
#include "database/database_export.h"
#include "sqlite3.h"

namespace Database {

namespace detail {

template <typename ValueT> struct is_optional : std::false_type {};

template <typename ValueT>
struct is_optional<std::optional<ValueT>> : std::true_type {
  typedef ValueT value_type;
};

template <typename ValueT>
auto getFromQuery(sqlite3_stmt *stmt, int idx) -> ValueT;

template <typename ValueT>
auto getOptionalFromQuery(sqlite3_stmt *stmt, int idx) -> ValueT
{
  if (sqlite3_column_type(stmt, idx) == SQLITE_NULL)
    return std::nullopt;

  return getFromQuery<typename ValueT::value_type>(stmt, idx);
}

} // namespace detail

class DATABASE_EXPORT Query {
public:
  Query(const std::string_view &sql, Connection &connection);
  virtual ~Query();

  auto execute() -> void;

  template <typename ValueT>
  auto get(const std::string_view &fieldName) -> ValueT {
    if constexpr (detail::is_optional<ValueT>::value)
    {
      return detail::getOptionalFromQuery<ValueT>(
          getRawStatement(), getColumnIdxFromStatement(fieldName));
    }

    return detail::getFromQuery<ValueT>(getRawStatement(),
                                        getColumnIdxFromStatement(fieldName));
  }

private:
  sqlite3_stmt *getRawStatement() const;
  int getColumnIdxFromStatement(const std::string_view &fieldName);

  class Impl;
  std::unique_ptr<Impl> m_impl;
};

} // namespace Database