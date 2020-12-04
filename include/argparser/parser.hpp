// ArgParse - A Command Line Argument Parser for C++
// Copyright (C) 2008-2020 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_ARGPARSER_PARSER_HPP
#define HEADER_ARGPARSER_PARSER_HPP

#include <iostream>
#include <memory>
#include <span>
#include <string_view>
#include <variant>
#include <vector>

#include "item.hpp"
#include "option_group.hpp"

namespace argparser {

class ParseContext;

class Parser : public OptionGroup
{
public:
  Parser();

  void add_program(std::string program);

  void print_usage(std::ostream& out = std::cout) const;
  void print_help(std::ostream& out = std::cout, uint32_t visibility_mask = ~0) const;
  void print_help(CommandItem const& command_item, std::ostream& out = std::cout, uint32_t visibility_mask = ~0) const;

  void parse_args(int argc, char const* const* argv);
  void parse_args(std::span<char const* const> argv);

private:
  void print_help(OptionGroup const& group, CommandItem const* current_command_item, std::ostream& out, uint32_t visibility_mask, bool skip_print_usage) const;
  void print_usage(CommandItem const* current_command_item, std::ostream& out) const;

  void parse_args(ParseContext& ctx, OptionGroup const& group, bool options_only = false);
  void parse_long_option(ParseContext& ctx, OptionGroup const& group, std::string_view arg);
  void parse_short_option(ParseContext& ctx, OptionGroup const& group, std::string_view arg);
  void parse_non_option(ParseContext& ctx, OptionGroup const& group, std::string_view arg);

  void verify(ParseContext& ctx, std::string_view arg, Item const& item);
  void check_mutual_exclusion(ParseContext& ctx, std::string_view arg, Item const& item);

private:
  std::string m_program;
};

} // namespace argparser

#endif

/* EOF */
