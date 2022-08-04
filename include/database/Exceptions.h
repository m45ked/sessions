#include <stdexcept>
#include <string_view>

#include "fmt/format-inl.h"

namespace Database {

class IncorrectQuerySql : public std::runtime_error {
public:
  IncorrectQuerySql(const std::string_view &msg)
      : std::runtime_error(fmt::format("Incorrect SQL: '%s'", msg)) {}
};

} // namespace Database
