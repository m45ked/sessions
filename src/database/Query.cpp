#include "Query.h"

#include <algorithm>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <locale>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "spdlog/fmt/bundled/format.h"
#include "spdlog/spdlog.h"
#include "sqlite/sqlite3.h"

#include "database/Connection.h"
#include "database/Exceptions.h"

namespace {

auto getLowerCaseString(std::string_view text) -> std::string {
  std::string lowercaseName = text.data();
  transform(begin(text), end(text), begin(lowercaseName),
            [=](const auto c) { return tolower(c, std::locale()); });
  return lowercaseName;
}

struct statement_deleter {
  auto operator()(sqlite3_stmt *statement) -> void {
    const auto rc = sqlite3_finalize(statement);
  }
};

} // namespace

namespace Database {

Query::~Query() {}

class Query::Impl {
public:
  Impl(std::string_view sql, sqlite3 *dbConnection);

  auto execute() -> void;
  auto getIndex(std::string_view fieldName) const -> int64_t;
  auto getStatement() const -> sqlite3_stmt *;

private:
  std::unique_ptr<sqlite3_stmt, statement_deleter> m_dbStatement;
  std::vector<std::string> m_columns;
};

Query::Impl::Impl(std::string_view sql, sqlite3 *dbConnection) {
  const char *outSql;
  sqlite3_stmt *statement;
  const auto result = sqlite3_prepare_v2(dbConnection, sql.data(), sql.size(),
                                         &statement, &outSql);
  m_dbStatement =
      std::unique_ptr<sqlite3_stmt, statement_deleter>(std::move(statement));
  if (result != SQLITE_OK) {
    throw QueryError(result, sqlite3_errmsg(dbConnection));
  }
}

auto Query::Impl::execute() -> void {
  const auto stmt = m_dbStatement.get();

  auto val = sqlite3_step(stmt);
  if (val != SQLITE_DONE) {
    while (sqlite3_step(stmt) != SQLITE_ROW) {
    }
  }
  if (val == SQLITE_ERROR) {
    auto db = sqlite3_db_handle(m_dbStatement.get());
    const auto errorStr = sqlite3_errmsg(db);
    const auto errorCode = sqlite3_errcode(db);

    throw QueryError(errorCode, errorStr);
  }

  spdlog::debug("Called \"{}\"\n", sqlite3_normalized_sql(stmt));

  const auto columnCount = sqlite3_column_count(stmt);
  for (auto i = 0; i < columnCount; ++i) {
    const auto name = getLowerCaseString({sqlite3_column_name(stmt, i)});

    spdlog::debug("Got column: '{}'\n", name);
    m_columns.emplace_back(name);
  }
}

auto Query::Impl::getIndex(std::string_view fieldName) const -> int64_t {
  const auto beginIt = begin(m_columns);
  const auto it = find(beginIt, end(m_columns), fieldName);
  return distance(beginIt, it);
}

auto Query::Impl::getStatement() const -> sqlite3_stmt * {
  return m_dbStatement.get();
}

Query::Query(std::string_view sql, Connection &connection)
    : m_impl(std::make_unique<Impl>(sql, connection.getRawConnection())) {}

auto Query::execute() -> void { m_impl->execute(); }

namespace detail {

template <> auto getFromQuery<double>(sqlite3_stmt *stmt, int idx) -> double {
  return sqlite3_column_double(stmt, idx);
}

template <> auto getFromQuery<int64_t>(sqlite3_stmt *stmt, int idx) -> int64_t {
  return sqlite3_column_int64(stmt, idx);
}

template <> auto getFromQuery<int>(sqlite3_stmt *stmt, int idx) -> int {
  return sqlite3_column_int(stmt, idx);
}

template <> auto getFromQuery(sqlite3_stmt *stmt, int idx) -> std::string {
  const auto text = sqlite3_column_text(stmt, idx);
  const std::size_t length = sqlite3_column_bytes(stmt, idx);
  return {reinterpret_cast<const char *>(text), length};
}

template <>
auto getFromQuery(sqlite3_stmt *stmt, int idx) -> std::vector<std::byte> {
  const auto value = static_cast<const char *>(sqlite3_column_blob(stmt, idx));
  const auto length = sqlite3_column_bytes(stmt, idx);
  auto vec = std::vector<std::byte>{static_cast<unsigned long>(length)};

  for (size_t i = 0; i < length; ++i) {
    vec[i] = static_cast<std::byte>(*(value + i));
  }

  return vec;
}

template <>
auto bindParameterValue(sqlite3_stmt *stmt, int idx, const int &value) -> void {
  fmt::print("idx: {} value", idx);
  const auto val = sqlite3_bind_int64(stmt, idx, value); // TODO Błędne kody
  if (val != SQLITE_OK)
    throw std::runtime_error(fmt::format("dupa {}", val));
}

template <>
auto bindParameterValue(sqlite3_stmt *stmt, int idx, const double &value)
    -> void {
  fmt::print("idx: {} value", idx);
  const auto val = sqlite3_bind_double(stmt, idx, value); // TODO Błędne kody
  if (val != SQLITE_OK)
    throw std::runtime_error(fmt::format("dupa {}", val));
}

template <>
auto bindParameterValue(sqlite3_stmt *stmt, int idx, const std::string &value)
    -> void {
  fmt::print("idx: {} value", idx);
  const auto val = sqlite3_bind_text(stmt, idx, value.data(), value.size(),
                                     SQLITE_TRANSIENT); // TODO Błędne kody
  if (val != SQLITE_OK)
    throw std::runtime_error(fmt::format("dupa {}", val));
}

template <>
auto bindParameterValue(sqlite3_stmt *stmt, int idx,
                        const std::string_view &value) -> void {
  fmt::print("idx: {} value", idx);
  const auto val = sqlite3_bind_text(stmt, idx, value.data(), value.size(),
                                     SQLITE_TRANSIENT); // TODO Błędne kody
  if (val != SQLITE_OK)
    throw std::runtime_error(fmt::format("dupa {}", val));
}

template <>
auto bindParameterValue(sqlite3_stmt *stmt, int idx, const char *const &value)
    -> void {
  bindParameterValue(stmt, idx, std::string{value});
}

template <>
auto bindParameterValue(sqlite3_stmt *stmt, int idx,
                        const std::vector<std::byte> &value) -> void {
  fmt::print("idx: {} value", idx);
  const auto val = sqlite3_bind_blob(stmt, idx, value.data(), value.size(),
                                     SQLITE_TRANSIENT); // TODO Błędne kody
  if (val != SQLITE_OK)
    throw std::runtime_error(fmt::format("dupa {}", val));
}

} // namespace detail

auto Query::getColumnIdxFromStatement(std::string_view fieldName) const -> int {
  return m_impl->getIndex(fieldName);
}

auto Query::getRawStatement() const -> sqlite3_stmt * {
  return m_impl->getStatement();
}

auto Query::getParmameterIndex(std::string_view parameterName) const -> int {
  const auto idx = sqlite3_bind_parameter_index(
      getRawStatement(), fmt::format(":{}", parameterName).c_str());
  if (!idx)
    throw NoSuchSqlParameter(parameterName);

  return idx;
}

} // namespace Database
