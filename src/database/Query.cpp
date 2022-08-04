#include "database/Query.h"
#include "database/Connection.h"
#include "database/Exceptions.h"

#include "sqlite/sqlite3.h"

#include "fmt/format-inl.h"

#include <algorithm>
#include <iostream>
#include <locale>
#include <string_view>
#include <type_traits>
#include <vector>

namespace {

int getIndex(const std::vector<std::string> &container,
             const std::string_view &fieldName) {
  const auto it =
      std::find(std::begin(container), std::end(container), fieldName);
  return std::distance(std::begin(container), it);
}

std::string getLowerCaseString(const std::string &text) {
  std::string lowercaseName = text;
  std::transform(std::begin(text), std::end(text), std::begin(lowercaseName),
                 [=](std::string::value_type c) {
                   return std::tolower(c, std::locale());
                 });
  return lowercaseName;
}

} // namespace

namespace Database {

class Query::Impl {
public:
  Impl(const std::string_view &sql, sqlite3 *dbConnection);
  ~Impl();

  std::string getString(const std::string_view &fieldName) const;
  double getDouble(const std::string_view &fieldName) const;
  int64_t getInteger(const std::string_view &fieldName) const;

  void execute();

private:
  sqlite3_stmt *m_dbStatement;
  std::vector<std::string> m_columns;
};

Query::Impl::Impl(const std::string_view &sql, sqlite3 *dbConnection) {
  const char *outSql;
  const auto result = sqlite3_prepare_v2(dbConnection, sql.data(), sql.size(),
                                         &m_dbStatement, &outSql);

  if (result != SQLITE_OK) {
    throw IncorrectQuerySql(sql);
  }
}

Query::Impl::~Impl() { const auto result = sqlite3_finalize(m_dbStatement); }

void Query::Impl::execute() {
  while (sqlite3_step(m_dbStatement) != SQLITE_ROW) {
  }

  // const auto result = sqlite3_step(m_dbStatement);
  std::cout << fmt::format("Called \"{}\"\n",
                           sqlite3_expanded_sql(m_dbStatement));

  const auto columnCount = sqlite3_column_count(m_dbStatement);
  for (auto i = 0; i < columnCount; ++i) {
    const auto name =
        getLowerCaseString(std::string{sqlite3_column_name(m_dbStatement, i)});

    std::cout << fmt::format("Got column: '{}'\n", name);
    m_columns.emplace_back(name);
  }
}

int64_t Query::Impl::getInteger(const std::string_view &fieldName) const {
  const auto index = getIndex(m_columns, fieldName);
  return sqlite3_column_int64(m_dbStatement, index);
}

double Query::Impl::getDouble(const std::string_view &fieldName) const {
  const auto index = getIndex(m_columns, fieldName);
  return sqlite3_column_double(m_dbStatement, index);
}

std::string Query::Impl::getString(const std::string_view &fieldName) const {
  const auto index = getIndex(m_columns, fieldName);
  const auto text = sqlite3_column_text(m_dbStatement, index);
  const std::size_t length = sqlite3_column_bytes(m_dbStatement, index);
  return {reinterpret_cast<const char *>(text), length};
}

Query::Query(const std::string_view &sql, Connection &connection)
    : m_impl(
          std::make_shared<Query::Impl>(sql, connection.getRawConnection())) {}

void Query::execute() { m_impl->execute(); }

int64_t Query::getInteger(const std::string_view &fieldName) {
  return m_impl->getInteger(fieldName);
}

double Query::getDouble(const std::string_view &fieldName) {
  return m_impl->getDouble(fieldName);
}

std::string Query::getString(const std::string_view &fieldName) {
  return m_impl->getString(fieldName);
}

} // namespace Database
