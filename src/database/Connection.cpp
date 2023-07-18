#include "Connection.h"

#include <memory>
#include <string_view>

#include "fmt/format.h"

#include "sqlite3.h"

#include "database/Exceptions.h"

namespace {

sqlite3 *createConnection(const std::string_view &connectionString,
                          const int customFlags = 0) {
  const auto flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | customFlags;
  sqlite3 *connection;
  const auto result =
      sqlite3_open_v2(connectionString.data(), &connection, flags, nullptr);
  if (result != SQLITE_OK) {
    throw Database::ErrorOpeningDatabase(
        fmt::format("Cannot open database '{}': error code {:x}",
                    connectionString, result));
  }
  return connection;
}

struct connection_deleter {
  auto operator()(sqlite3 *ptr) -> void {
    [[maybe_unused]] const auto rc = sqlite3_close(ptr);
  }
};

} // namespace

namespace Database {

class Connection::Impl {
public:
  Impl(const std::string_view &connectionString);
  Impl();

  virtual ~Impl();

  sqlite3 *getRawConnection() const;

private:
  std::unique_ptr<sqlite3, connection_deleter> m_dbConnection;
};

Connection::Impl::~Impl() {}

Connection::Impl::Impl(const std::string_view &connectionString)
    : m_dbConnection(std::move(createConnection(connectionString))) {}

Connection::Impl::Impl()
    : m_dbConnection(
          std::move(createConnection(":memory:", SQLITE_OPEN_MEMORY))) {}

sqlite3 *Connection::Impl::getRawConnection() const {
  return m_dbConnection.get();
}

Connection::Connection() : m_impl(std::make_unique<Impl>()) {}

Connection::~Connection() {}

Connection::Connection(const std::string_view &connectionString)
    : m_impl(std::make_unique<Impl>(connectionString)) {}

sqlite3 *Connection::getRawConnection() const {
  return m_impl->getRawConnection();
}

} // namespace Database
