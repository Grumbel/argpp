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

#ifndef HEADER_ARGPARSER_ARGPARSER_HPP
#define HEADER_ARGPARSER_ARGPARSER_HPP

#include <iostream>
#include <memory>
#include <string_view>
#include <variant>
#include <vector>

#include "item.hpp"
#include "option_group.hpp"

namespace argparser {

class ParseContext;
class OptionGroup;

class ArgParser
{
public:
  ArgParser();

  void add_usage(std::string_view program, std::string_view usage);
  OptionGroup& options();

  void print_help(std::ostream& out = std::cout) const;
  void parse_args(int argc, char** argv);

private:
  void parse_long_option(ParseContext& ctx, std::string_view arg);
  void parse_short_option(ParseContext& ctx, std::string_view arg);
  void parse_non_option(ParseContext& ctx, std::string_view arg);

private:
  std::string m_program;
  std::string m_usage;
  OptionGroup m_root;

private:
  ArgParser(const ArgParser&) = delete;
  ArgParser& operator=(const ArgParser&) = delete;
};

} // namespace argparser

#endif

/* EOF */
