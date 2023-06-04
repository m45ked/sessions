#pragma once

#include <cassert>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>


#include "database/Connection_fwd.h"
#include "database/Query_fwd.h"
#include "database/database_export.h"
#include "sqlite3.h"

namespace Database {

template<typename ValueT> auto getFromQuery(sqlite3_stmt* stmt, int idx) -> ValueT;

class DATABASE_EXPORT Query {
public:
  Query(const std::string_view &sql, Connection &connection);
  auto execute() -> void;

  template <typename ValueT> ValueT get(const std::string_view &fieldName)
  {
    return getFromQuery<ValueT>(getRawStatement(), getColumnIdxFromStatement(fieldName));
  }

private:
  sqlite3_stmt* getRawStatement() const;
  int getColumnIdxFromStatement(const std::string_view& fieldName);

  class Impl;
  friend class Impl;
  std::shared_ptr<Impl> m_impl;
};

} // namespace Database