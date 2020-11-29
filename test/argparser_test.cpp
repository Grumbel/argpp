#include <gtest/gtest.h>

#include <string_view>

#include <argparser/argparser.hpp>

using namespace argparser;

namespace {

class Mock
{
public:
  Mock() : m_counter() {}
  void call(std::string const& name) { m_counter[name] += 1; }
  int times_called(std::string const& name) { return m_counter[name]; }

private:
  std::map<std::string, int> m_counter;
};

void run_test(std::span<char const* const> argv, Mock& mock)
{
  ArgParser argp;
  argp.add_option('h', "help", "Display help").then([&]{ mock.call("help"); });
  argp.add_positional(Argument<std::string>("POS1")).then([&](std::string){ mock.call("pos1"); });
  argp.add_positional(Argument<std::string>("POS2")).then([&](std::string){ mock.call("pos2"); });
  argp.add_rest(Argument<std::string>("REST")).then([&](std::string){ mock.call("rest"); });

  argp.parse_args(argv);
}

} // namespace

TEST(ArgParserTest, add_option__short)
{
  Mock mock;
  run_test({{"example", "-h"}}, mock);
  EXPECT_EQ(mock.times_called("help"), 1);
}

TEST(ArgParserTest, add_option__long)
{
  Mock mock;
  run_test({{"example", "--help"}}, mock);
  EXPECT_EQ(mock.times_called("help"), 1);
}

TEST(ArgParserTest, add_rest)
{
  Mock mock;
  run_test({{"example", "pos1", "pos2", "rest1", "rest2", "rest3"}}, mock);
  EXPECT_EQ(mock.times_called("rest"), 3);
}

TEST(ArgParserTest, add_positional)
{
  Mock mock;
  run_test({{"example", "pos1", "pos2"}}, mock);
  EXPECT_EQ(mock.times_called("pos1"), 1);
  EXPECT_EQ(mock.times_called("pos2"), 1);
}

/* EOF */
