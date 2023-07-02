#include "Query.h"

#include <algorithm>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <locale>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "sqlite/sqlite3.h"

#include "fmt/format.h"

#include "database/Connection.h"
#include "database/Exceptions.h"

namespace {

auto getLowerCaseString(const std::string_view &text) -> std::string {
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
  Impl(const std::string_view &sql, sqlite3 *dbConnection);

  auto execute() -> void;
  auto getIndex(const std::string_view &fieldName) const -> int64_t;
  auto getStatement() const -> sqlite3_stmt *;

private:
  std::unique_ptr<sqlite3_stmt, statement_deleter> m_dbStatement;
  std::vector<std::string> m_columns;
};

Query::Impl::Impl(const std::string_view &sql, sqlite3 *dbConnection) {
  const char *outSql;
  sqlite3_stmt *statement;
  const auto result = sqlite3_prepare_v2(dbConnection, sql.data(), sql.size(),
                                         &statement, &outSql);
  m_dbStatement =
      std::unique_ptr<sqlite3_stmt, statement_deleter>(std::move(statement));
  if (result != SQLITE_OK) {
    throw IncorrectQuerySql(sql);
  }
}

auto Query::Impl::execute() -> void {
  const auto stmt = m_dbStatement.get();
  while (sqlite3_step(stmt) != SQLITE_ROW) {
  }

  std::cout << fmt::format("Called \"{}\"\n", sqlite3_expanded_sql(stmt));

  const auto columnCount = sqlite3_column_count(stmt);
  for (auto i = 0; i < columnCount; ++i) {
    const auto name = getLowerCaseString({sqlite3_column_name(stmt, i)});

    std::cout << fmt::format("Got column: '{}'\n", name);
    m_columns.emplace_back(name);
  }
}

auto Query::Impl::getIndex(const std::string_view &fieldName) const -> int64_t {
  const auto beginIt = begin(m_columns);
  const auto it = find(beginIt, end(m_columns), fieldName);
  return distance(beginIt, it);
}

auto Query::Impl::getStatement() const -> sqlite3_stmt * {
  return m_dbStatement.get();
}

Query::Query(const std::string_view &sql, Connection &connection)
    : m_impl(std::make_unique<Impl>(sql, connection.getRawConnection())) {}

auto Query::execute() -> void { m_impl->execute(); }

template <> auto getFromQuery<double>(sqlite3_stmt *stmt, int idx) -> double {
  return sqlite3_column_double(stmt, idx);
}

template <> auto getFromQuery<int64_t>(sqlite3_stmt *stmt, int idx) -> int64_t {
  return sqlite3_column_int64(stmt, idx);
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

int Query::getColumnIdxFromStatement(const std::string_view &fieldName) {
  return m_impl->getIndex(fieldName);
}

sqlite3_stmt *Query::getRawStatement() const { return m_impl->getStatement(); }

} // namespace Database
