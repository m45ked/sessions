#pragma once

#include <string_view>

#include "database/Connection.h"
#include "database/database_export.h"

namespace Database {

bool DATABASE_EXPORT isTableExist(Connection &conn, std::string_view tableName);

}