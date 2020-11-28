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

#ifndef HEADER_ARGPARSER_OPTION_GROUP_HPP
#define HEADER_ARGPARSER_OPTION_GROUP_HPP

#include <string_view>
#include <memory>
#include <vector>
#include <unordered_map>

#include "error.hpp"
#include "option.hpp"
#include "positional_item.hpp"
#include "rest_item.hpp"

namespace argparser {

class CommandItem;
class Option;
class PositionalItem;

class OptionGroup
{
public:
  OptionGroup();
  ~OptionGroup() = default;

  void add_text(std::string text);
  void add_newline();
  void add_group(std::string name);

  void add_pseudo(std::string name, std::string help);

  void add_alias(char name, Option const& option);
  void add_alias(std::string name, Option const& option);

  CommandItem& add_command(std::string name, std::string help);

  template<typename T>
  TPositionalItem<T>& add_positional(Argument<T> argument, std::string help = {})
  {
    auto positional_item = std::make_unique<TPositionalItem<T>>(std::move(argument), std::move(help));
    auto& positional_item_ref = *positional_item;
    m_items.emplace_back(std::move(positional_item));
    return positional_item_ref;
  }

  template<typename T>
  TRestItem<T>& add_rest(Argument<T> argument, std::string help = {})
  {
    if (has_rest()) {
      throw Error("only one rest argument allowed");
    }

    auto rest_item = std::make_unique<TRestItem<T>>(std::move(argument), std::move(help));
    auto& rest_item_ref = *rest_item;
    m_items.emplace_back(std::move(rest_item));
    return rest_item_ref;
  }

  OptionWithoutArg& add_option(char short_name, std::string long_name, std::string help)
  {
    auto opt = std::make_unique<OptionWithoutArg>(short_name, std::move(long_name), std::move(help));
    return dynamic_cast<OptionWithoutArg&>(add_option(std::move(opt)));
  }

  template<typename T>
  TOptionWithArg<T>& add_option(char short_name, std::string long_name, Argument<T> argument, std::string help)
  {
    auto opt = std::make_unique<TOptionWithArg<T>>(short_name, std::move(long_name), std::move(argument), std::move(help));
    return dynamic_cast<TOptionWithArg<T>&>(add_option(std::move(opt)));
  }

  CommandItem& lookup_command(std::string_view name) const;
  PositionalItem& lookup_positional(int i) const;
  Option const& lookup_short_option(char name) const;
  Option const& lookup_long_option(std::string_view name) const;

  bool has_options() const;
  bool has_commands() const;
  bool has_positional() const;
  bool has_rest() const;

  std::vector<std::unique_ptr<Item> > const& get_items() const { return m_items; }

private:
  Option& add_option(std::unique_ptr<Option> option);
  void add_short_option(char name, Option const& option);
  void add_long_option(std::string name, Option const& option);

private:
  std::vector<std::unique_ptr<Item> > m_items;
  std::unordered_map<char, Option const*> m_short_options;
  std::unordered_map<std::string, Option const*> m_long_options;
};

} // namespace argparser

#endif

/* EOF */
