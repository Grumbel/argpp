#ifndef HEADER_PARSER_HPP
#define HEADER_PARSER_HPP

#include <assert.h>
#include <string_view>

class Grammar
{
public:
  Grammar();

  Option& lookup_positional(int i);
  Option& lookup_short(char c);
  Option& lookup_long(std::string_view);

  void add_option();

private:
  std::vector<std::unique_ptr<Option>> m_positonal;
  std::vector<std::unique_ptr<Option>> m_rest;
  std::vector<std::unique_ptr<Option>> m_options;
};

class Parser
{
public:
  Parser(Grammar const& grammar);

  void parse(int argc, char** argv);
  void parse(std::span<char const*> argv);

private:

private:
  Parser(const Parser&) = delete;
  Parser& operator=(const Parser&) = delete;
};

#endif

/* EOF */
