#include "database/Connection.h"

#include <string_view>

#include "fmt/format.h"

#include "sqlite3.h"
// #include "sqlite3ext.h"

namespace Database {

class Connection::Impl {
public:
  Impl(const std::string_view &connectionString);
  Impl();
  ~Impl();

  sqlite3 *getRawConnection();

private:
  sqlite3 *m_dbConnection;
};

Connection::Impl::Impl(const std::string_view &connectionString) {
  const auto flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  const auto result =
      sqlite3_open_v2(connectionString.data(), &m_dbConnection, flags, nullptr);
  if (result != SQLITE_OK) {
    throw std::runtime_error(
        fmt::format("Cannot open database '{}': error code {:x}",
                    connectionString, result));
  }
}

Connection::Impl::Impl() {
  const auto flags =
      SQLITE_OPEN_MEMORY | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  const auto result =
      sqlite3_open_v2(":memory:", &m_dbConnection, flags, nullptr);
  if (result != SQLITE_OK) {
    throw std::runtime_error(fmt::format(
        "Cannot open database '{}': error code {:x}", ":memory:", result));
  }
}

Connection::Impl::~Impl() { sqlite3_close(m_dbConnection); }

sqlite3 *Connection::Impl::getRawConnection() { return m_dbConnection; }

Connection::Connection() : m_impl(std::make_shared<Connection::Impl>("")) {}

Connection::Connection(const std::string_view &connectionString)
    : m_impl(std::make_shared<Connection::Impl>(connectionString)) {}

sqlite3 *Connection::getRawConnection() { return m_impl->getRawConnection(); }

} // namespace Database
