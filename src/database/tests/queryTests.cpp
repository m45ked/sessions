#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

#include "database/Connection.h"
#include "database/Exceptions.h"
#include "database/Query.h"
#include "gmock/gmock-more-matchers.h"
#include "gmock/gmock.h"
#include "gtest/gtest-matchers.h"
#include "gtest/gtest.h"

namespace {

using Q = Database::Query;

class QueryTest : public ::testing::Test {
protected:
  Database::Connection m_conn;
};

TEST_F(QueryTest, createSimpleQuery) {
  auto query = Q{R"sql(select 1 'id')sql", m_conn};
  query.execute();
}

TEST_F(QueryTest, createIncorrectQuery) {
  EXPECT_THROW(Q(R"sql(select)sql", m_conn), Database::IncorrectQuerySql);
}

TEST_F(QueryTest, getColumnValue_int) {
  auto query = Q(R"sql(select 1 id)sql", m_conn);
  query.execute();

  EXPECT_EQ(query.get<int64_t>("id"), 1);
}

TEST_F(QueryTest, getColumnValue_double) {
  auto query = Q(R"sql(select 1.5 id)sql", m_conn);
  query.execute();

  EXPECT_EQ(query.get<double>("id"), 1.5);
}

TEST_F(QueryTest, getColumnValue_string) {
  auto query = Q(R"sql(select "field value" id)sql", m_conn);
  query.execute();

  EXPECT_EQ(query.get<std::string>("id"), "field value");
}

TEST_F(QueryTest, getColumnValue_blob) {
  auto query = Q(R"sql(select randomblob(16) id)sql", m_conn);
  query.execute();
  auto f = query.get<std::vector<std::byte>>("id");
  const auto defaultValue = std::vector<std::byte>{16ul};
  EXPECT_THAT(f, ::testing::Not(::testing::Eq(defaultValue)));
  EXPECT_THAT(f.size(), ::testing::Eq(defaultValue.size()));
}

TEST_F(QueryTest, getColumnValue_optInt) {
  auto query = Q{R"sql(select null 'value')sql", m_conn};
  query.execute();

  EXPECT_THAT(query.get<std::optional<int64_t>>("value"),
              ::testing::Eq(std::nullopt));
}

TEST_F(QueryTest, getColumnValue_optString) {
  auto query = Q{R"sql(select null 'value')sql", m_conn};
  query.execute();

  EXPECT_THAT(query.get<std::optional<std::string>>("value"),
              ::testing::Eq(std::nullopt));
}

TEST_F(QueryTest, getColumnValue_optDouble) {
  auto query = Q{R"sql(select null 'value')sql", m_conn};
  query.execute();

  EXPECT_THAT(query.get<std::optional<double>>("value"),
              ::testing::Eq(std::nullopt));
}

TEST_F(QueryTest, getColumnValue_optBlob) {
  auto query = Q{R"sql(select null 'value')sql", m_conn};
  query.execute();

  EXPECT_THAT(query.get<std::optional<std::vector<std::byte>>>("value"),
              ::testing::Eq(std::nullopt));
}

TEST_F(QueryTest, setColumnValue_int) {
  auto query = Q{R"sql(select :val 'value')sql", m_conn};
  query.set("val", 1);
  query.execute();

  EXPECT_THAT(query.get<int64_t>("value"), ::testing::Eq(1));
}

TEST_F(QueryTest, setColumnValue_double) {
  auto query = Q{R"sql(select :val 'value')sql", m_conn};
  query.set("val", 3.1514);
  query.execute();

  EXPECT_THAT(query.get<double>("value"), ::testing::Eq(3.1514));
}

TEST_F(QueryTest, setColumnValue_string) {
  auto query = Q{R"sql(select :val 'value')sql", m_conn};
  query.set("val", std::string{"dupa"});
  query.execute();

  EXPECT_THAT(query.get<std::string>("value"), ::testing::Eq("dupa"));
}

TEST_F(QueryTest, setColumnValue_blob) {
  const auto value = std::vector<std::byte>{std::byte{0b001}, std::byte{0b101},
                                            std::byte{0b110}};
  auto query = Q{R"sql(select :val 'value')sql", m_conn};
  query.set("val", value);
  query.execute();

  EXPECT_THAT(query.get<std::vector<std::byte>>("value"), ::testing::Eq(value));
}

TEST_F(QueryTest, setColumnValue_optInt) {
  auto query = Q{R"sql(select :val 'value')sql", m_conn};
  query.set("val", std::optional<int>{});
  query.execute();

  EXPECT_THAT(query.get<std::optional<int64_t>>("value"),
              ::testing::Eq(std::nullopt));
}

TEST_F(QueryTest, setColumnValue_optDouble) {
  auto query = Q{R"sql(select :val 'value')sql", m_conn};
  query.set("val", std::optional<double>{});
  query.execute();

  EXPECT_THAT(query.get<std::optional<double>>("value"),
              ::testing::Eq(std::nullopt));
}

TEST_F(QueryTest, setColumnValue_optString) {
  auto query = Q{R"sql(select :val 'value')sql", m_conn};
  query.set("val", std::optional<std::string>{});
  query.execute();

  EXPECT_THAT(query.get<std::optional<std::string>>("value"),
              ::testing::Eq(std::nullopt));
}

TEST_F(QueryTest, setColumnValue_optBlob) {
  auto query = Q{R"sql(select :val 'value')sql", m_conn};
  query.set("val", std::optional<std::vector<std::byte>>{});
  query.execute();

  EXPECT_THAT(query.get<std::optional<std::vector<std::byte>>>("value"),
              ::testing::Eq(std::nullopt));
}

} // namespace