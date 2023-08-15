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
  virtual ~Connection();
  explicit Connection(std::string_view connectionString);

private:
  class Impl;

  friend class Impl;
  friend class Query;

  auto getRawConnection() const -> sqlite3 *;

  std::unique_ptr<Impl> m_impl;
};

} // namespace Database
