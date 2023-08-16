#include "gmock/gmock.h"
#include <gtest/gtest.h>

#include "database/Connection.h"
#include "database/Query.h"
#include "database/isTableExist.h"

namespace {

class isTableExistsTest : public testing::Test {
protected:
  Database::Connection m_conn;
};

TEST_F(isTableExistsTest, tableNotExists) {
  ASSERT_THAT(Database::isTableExist(m_conn, "foo"), testing::IsFalse());
}

TEST_F(isTableExistsTest, tableExists) {
  Database::Query{"create table foo ( id number )", m_conn}.execute();
  ASSERT_THAT(Database::isTableExist(m_conn, "foo"), testing::IsTrue());
}


} // namespace