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

#ifndef HEADER_ARGPARSER_OPTION_GROUP_HPP
#define HEADER_ARGPARSER_OPTION_GROUP_HPP

#include <string_view>
#include <memory>
#include <vector>

#include "item.hpp"

namespace argparser {

class OptionGroup
{
public:
  OptionGroup() : m_items() {}
  ~OptionGroup() {}

  void add_text(std::string_view text);
  void add_newline();
  void add_group(std::string_view name);

  void add_alias(std::string name, Option& option);
  void add_alias(char name, Option& option);

  Option& add_option(std::unique_ptr<Option> option);

  Option& add_option(char short_name, std::string long_name, std::string help)
  {
    std::unique_ptr<Option> option(new Option(short_name, long_name, {}, help));
    return add_option(std::move(option));
  }

  template<typename T>
  Option& add_option(char short_name, std::string long_name, Argument<T> argument, std::string help)
  {
    return add_option(std::unique_ptr<Option>(new Option(
                                                short_name, std::move(long_name),
                                                std::make_unique<Argument<T>>(std::move(argument)),
                                                help)));
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

  PositionalItem& lookup_positional(int i);
  Option& lookup_short_option(char name);
  Option& lookup_long_option(std::string_view name);

  std::vector<std::unique_ptr<Item> > const& get_items() const { return m_items; }

private:
  std::vector<std::unique_ptr<Item> > m_items;
};

} // namespace argparser

#endif

/* EOF */
