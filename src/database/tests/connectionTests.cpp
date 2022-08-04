#include "database/Connection.h"
#include "gtest/gtest.h"

#include <filesystem>

namespace {

TEST(ConnectionTests, createDefaultConnection) {
  auto conn = Database::Connection();
}

TEST(ConnectionTests, createConnectionWithString) {
  auto conn = Database::Connection("temp.db3");
  std::filesystem::remove("temp.db3");
}

} // namespace
