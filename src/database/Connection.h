#pragma once

#include <memory>
#include <string_view>

#include "sqlite3.h"

#include "database/Query_fwd.h"
#include "database/database_export.h"

namespace Database {

class DATABASE_EXPORT Connection {
public:
  Connection();
  explicit Connection(const std::string_view &connectionString);

private:
  class Impl;
  friend class Impl;

  friend class Query;

  sqlite3 *getRawConnection() const;

  std::shared_ptr<Impl> m_impl;
};

} // namespace Database
