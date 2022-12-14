#include <algorithm>
#include <iostream>
#include <locale>
#include <string_view>
#include <type_traits>
#include <vector>

#include "sqlite/sqlite3.h"

#include "fmt/format.h"

#include "database/Connection.h"
#include "database/Exceptions.h"
#include "database/Query.h"

namespace {

std::string getLowerCaseString(const std::string_view &text) {
  std::string lowercaseName = text.data();
  transform(begin(text), end(text), begin(lowercaseName),
            [=](const auto c) { return tolower(c, std::locale()); });
  return lowercaseName;
}

} // namespace

namespace Database {

class Query::Impl {
public:
  Impl(const std::string_view &sql, sqlite3 *dbConnection);

  std::string getString(const std::string_view &fieldName) const;
  double getDouble(const std::string_view &fieldName) const;
  int64_t getInteger(const std::string_view &fieldName) const;
  const void *getBlob(const std::string_view &fieldName) const;

  void execute();

private:
  int getIndex(const std::string_view &fieldName) const;

  std::shared_ptr<sqlite3_stmt> m_dbStatement;
  std::vector<std::string> m_columns;
};

Query::Impl::Impl(const std::string_view &sql, sqlite3 *dbConnection) {
  const char *outSql;
  sqlite3_stmt *statement;
  const auto result = sqlite3_prepare_v2(dbConnection, sql.data(), sql.size(),
                                         &statement, &outSql);
  m_dbStatement = {statement, &sqlite3_finalize};
  if (result != SQLITE_OK) {
    throw IncorrectQuerySql(sql);
  }
}

void Query::Impl::execute() {
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

int64_t Query::Impl::getInteger(const std::string_view &fieldName) const {
  const auto index = getIndex(fieldName);
  return sqlite3_column_int64(m_dbStatement.get(), index);
}

double Query::Impl::getDouble(const std::string_view &fieldName) const {
  const auto index = getIndex(fieldName);
  return sqlite3_column_double(m_dbStatement.get(), index);
}

std::string Query::Impl::getString(const std::string_view &fieldName) const {
  const auto index = getIndex(fieldName);
  const auto text = sqlite3_column_text(m_dbStatement.get(), index);
  const std::size_t length = sqlite3_column_bytes(m_dbStatement.get(), index);
  return {reinterpret_cast<const char *>(text), length};
}

const void* Query::Impl::getBlob(const std::string_view& fieldName) const {
  const auto index = getIndex(fieldName);
  const auto value = sqlite3_column_blob(m_dbStatement.get(), index);
  return value;
}

int Query::Impl::getIndex(const std::string_view &fieldName) const {
  const auto beginIt = begin(m_columns);
  const auto it = find(beginIt, end(m_columns), fieldName);
  return distance(beginIt, it);
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

const void* Query::getBlob(const std::string_view& fieldName) {
  return m_impl->getBlob(fieldName);
}

} // namespace Database
