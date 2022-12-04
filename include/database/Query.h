#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

#include "database/Connection_fwd.h"
#include "database/Query_fwd.h"
#include "database/database_export.h"

namespace Database {

class DATABASE_EXPORT Query {
public:
  Query(const std::string_view &sql, Connection &connection);
  void execute();
  template <typename ValueT>
  inline ValueT get(const std::string_view &fieldName) {
    if constexpr (std::is_integral_v<ValueT>)
      return getInteger(fieldName);
    else if constexpr (std::is_floating_point_v<ValueT>)
      return getDouble(fieldName);
    else if constexpr (std::is_same_v<ValueT, std::string>)
      return getString(fieldName);
    else if constexpr (std::is_same_v<ValueT, const void *>)
      return getBlob(fieldName);

    assert(std::is_trivially_constructible_v<ValueT>);
    return {};
  }

private:
  double getDouble(const std::string_view &field);
  int64_t getInteger(const std::string_view &field);
  std::string getString(const std::string_view &field);
  const void * getBlob(const std::string_view &field);

  class Impl;
  friend class Impl;
  std::shared_ptr<Impl> m_impl;
};

} // namespace Database