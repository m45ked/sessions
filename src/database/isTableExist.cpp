#include "isTableExist.h"

#include <cstdint>
#include <string>

#include "database/Query.h"
#include "spdlog/fmt/bundled/core.h"

namespace Database {

bool isTableExist(Connection &conn, std::string_view tableName) {
  auto q = Query(
      R"sql(select count(1) 'c' from sqlite_master where type='table' and name=:tableName)sql",
      conn);
  q.set<std::string>("tableName", tableName.data());
  q.execute();

  return q.get<std::int64_t>("c") > 0;
}

} // namespace Database
