#include "database/Connection.h"
#include "database/Exceptions.h"
#include "database/Query.h"
#include "gtest/gtest.h"

namespace {

struct QueryTest : public ::testing::Test {
protected:
  Database::Connection m_conn;
};

TEST_F(QueryTest, createSimpleQuery) {
  auto query = Database::Query{R"sql(select 1 'id')sql", m_conn};
  query.execute();
}

TEST_F(QueryTest, createIncorrectQuery) {
  EXPECT_THROW(Database::Query(R"sql(select)sql", m_conn),
               Database::IncorrectQuerySql);
}

TEST_F(QueryTest, getColumnValue_int) {
  auto query = Database::Query(R"sql(select 1 id)sql", m_conn);
  query.execute();

  EXPECT_EQ(query.get<int>("id"), 1);
}

TEST_F(QueryTest, getColumnValue_double) {
  auto query = Database::Query(R"sql(select 1.5 id)sql", m_conn);
  query.execute();

  EXPECT_EQ(query.get<double>("id"), 1.5);
}

TEST_F(QueryTest, getColumnValue_string) {
  auto query = Database::Query(R"sql(select "field value" id)sql", m_conn);
  query.execute();

  EXPECT_EQ(query.get<std::string>("id"), "field value");
}

} // namespace
