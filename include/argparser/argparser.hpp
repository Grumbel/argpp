// ArgParse - A Command Line Argument Parser for C++
// Copyright (C) 2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_ARGPARSER_ARGPARSER_HPP
#define HEADER_ARGPARSER_ARGPARSER_HPP

#include <iostream>
#include <memory>
#include <string_view>
#include <variant>
#include <vector>

#include "item.hpp"

namespace argparser {

class ParseContext;

using Item = std::variant<TextItem, CommandItem>;

class OptionGroup
{
public:
  OptionGroup() : m_items() {}
  ~OptionGroup() {}

  void add_text(std::string_view text);
  void add_newline();
  Option& add_option(std::unique_ptr<Option> option);
  void add_alias(std::string alias, std::string_view name);
  void add_alias(char alias, char name);

  template<typename T>
  Option& add_option(char short_name, std::string_view long_name, Argument<T> argument, std::string help)
  {
    auto option = std::make_unique<Option>(short_name, long_name,
                             std::make_unique<Argument<T>>(std::move(argument)),
                             help);
    return add_option(option);
  }

  /*
  template<typename T>
  Option& add_option(std::string_view long_name, Argument<T> argument, std::string help)
  {

  }

  template<typename T>
  Option& add_option(char short_name, std::string_view long_name, std::string help)
  {
  }

  template<typename T>
  Option& add_option(std::string_view long_name, std::string help)
  {
  }
  */

private:
  std::vector<Item> m_items;
};

class ArgParser
{
public:
  ArgParser();

  void add_usage(std::string_view program, std::string_view usage);
  OptionGroup& add_group(std::string_view name = {});
  void add_command(std::string_view name, ArgParser argp);

  void print_help(std::ostream& out = std::cout) const;
  void parse_args(int argc, char** argv);

private:
  void parse_long_option(ParseContext& ctx, std::string_view arg);
  void parse_short_option(ParseContext& ctx, std::string_view arg);

  PositionalItem& lookup_positional(int i);
  Option& lookup_short_option(char c);
  Option& lookup_long_option(std::string_view);

private:
  std::vector<Item> m_items;

private:
  ArgParser(const ArgParser&) = delete;
  ArgParser& operator=(const ArgParser&) = delete;
};

} // namespace argparser

#endif

/* EOF */
