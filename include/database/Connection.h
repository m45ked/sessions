#pragma once

#include <memory>
#include <string_view>

#include "database/Query_fwd.h"
#include "sqlite3.h"

namespace Database {

class Connection {
public:
  Connection();
  Connection(const std::string_view &connectionString);

private:
  class Impl;
  friend class Impl;

  friend class Query;

  sqlite3 *getRawConnection();

  std::shared_ptr<Impl> m_impl;
};

} // namespace Database
