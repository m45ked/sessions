#pragma once

#include <stdexcept>
#include <string_view>

namespace Database {

class DatabaseRuntimeError : public std::runtime_error {
public:
  DatabaseRuntimeError(std::string_view msg) : std::runtime_error(msg.data()) {}
};

class IncorrectQuerySql : public DatabaseRuntimeError {
public:
  IncorrectQuerySql(std::string_view msg) : DatabaseRuntimeError(msg.data()) {}
};

class ErrorOpeningDatabase : public DatabaseRuntimeError {
public:
  ErrorOpeningDatabase(std::string_view msg)
      : DatabaseRuntimeError(msg.data()) {}
};

class NoSuchSqlParameter : public DatabaseRuntimeError {
public:
  NoSuchSqlParameter(std::string_view parameterName)
      : DatabaseRuntimeError(""), parameterName(parameterName) {}

  std::string parameterName;
};

struct QueryError : public DatabaseRuntimeError {
  QueryError(int errorCode, std::string_view msg)
      : DatabaseRuntimeError(msg.data()), errorCode(errorCode) {}

  int errorCode;
};

} // namespace Database
