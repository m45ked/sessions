#include "gmock/gmock.h"
#include <gtest/gtest.h>

#include "database/Connection.h"
#include "database/Query.h"
#include "database/isTableExist.h"

namespace {

TEST(isTableExistsTest, table_not_Exists) {
  Database::Connection m_conn;
  ASSERT_THAT(Database::isTableExist(m_conn, "foo"), testing::IsFalse());
  Database::Query{"create table foo ( id number )", m_conn}.execute();
  ASSERT_THAT(Database::isTableExist(m_conn, "foo"), testing::IsTrue());
}

} // namespace