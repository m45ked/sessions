#pragma once

#include <stdexcept>
#include <string_view>

namespace Database {

class IncorrectQuerySql : public std::runtime_error {
public:
  IncorrectQuerySql(std::string_view msg)
      : std::runtime_error(msg.data()) {}
};

class ErrorOpeningDatabase : public std::runtime_error {
public:
  ErrorOpeningDatabase(std::string_view msg)
      : std::runtime_error(msg.data()) {}
};

} // namespace Database
