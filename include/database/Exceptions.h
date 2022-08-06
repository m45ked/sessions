#pragma once

#include <stdexcept>
#include <string_view>

namespace Database {

class IncorrectQuerySql : public std::runtime_error {
public:
  IncorrectQuerySql(const std::string_view &msg)
      : std::runtime_error(msg.data()) {}
};

} // namespace Database
