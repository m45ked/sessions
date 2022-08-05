#include "database/Connection.h"

#include <string_view>

#include "fmt/format.h"

#include "sqlite3.h"

namespace Database {

class Connection::Impl {
public:
  Impl(const std::string_view &connectionString);
  Impl();

  sqlite3 *getRawConnection();

private:
  std::shared_ptr<sqlite3> m_dbConnection;
};

Connection::Impl::Impl(const std::string_view &connectionString) {
  const auto flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  sqlite3 *connection;
  const auto result =
      sqlite3_open_v2(connectionString.data(), &connection, flags, nullptr);
  if (result != SQLITE_OK) {
    throw std::runtime_error(
        fmt::format("Cannot open database '{}': error code {:x}",
                    connectionString, result));
  }
  m_dbConnection = std::shared_ptr<sqlite3>{connection, &sqlite3_close};
}

Connection::Impl::Impl() {
  const auto flags =
      SQLITE_OPEN_MEMORY | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  sqlite3 *connection;
  const auto result = sqlite3_open_v2(":memory:", &connection, flags, nullptr);
  if (result != SQLITE_OK) {
    throw std::runtime_error(fmt::format(
        "Cannot open database '{}': error code {:x}", ":memory:", result));
  }
  m_dbConnection = std::shared_ptr<sqlite3>{connection, &sqlite3_close};
}

sqlite3 *Connection::Impl::getRawConnection() { return m_dbConnection.get(); }

Connection::Connection() : m_impl(std::make_shared<Connection::Impl>()) {}

Connection::Connection(const std::string_view &connectionString)
    : m_impl(std::make_shared<Connection::Impl>(connectionString)) {}

sqlite3 *Connection::getRawConnection() { return m_impl->getRawConnection(); }

} // namespace Database
