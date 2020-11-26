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

#include "option_group.hpp"

namespace argparser {

void
OptionGroup::add_group(std::string_view name)
{
  m_items.push_back(std::make_unique<TextItem>(std::string(name)));
}

void
OptionGroup::add_text(std::string_view text)
{
  m_items.push_back(std::make_unique<TextItem>(std::string(text)));
}

void
OptionGroup::add_newline()
{
  m_items.push_back(std::make_unique<TextItem>(""));
}

Option&
OptionGroup::add_option(std::unique_ptr<Option> option)
{
  Option& option_ref = *option;
  m_items.push_back(std::move(option));
  return option_ref;
}

void
OptionGroup::add_alias(std::string name, Option& option)
{
  m_items.push_back(std::make_unique<LongOptionAlias>(name, option));
}

void
OptionGroup::add_alias(char name, Option& option)
{
  m_items.push_back(std::make_unique<ShortOptionAlias>(name, option));
}

PositionalItem&
OptionGroup::lookup_positional(int i)
{
  int positional_count = 0;
  for (auto& item : m_items) {
    Item* ptr = item.get();
    if (PositionalItem* positional_item = dynamic_cast<PositionalItem*>(ptr)) {
      if (positional_count == i) {
        return *positional_item;
      }
      positional_count += 1;
    }
  }
  throw std::runtime_error("positional item not found");
}

Option&
OptionGroup::lookup_short_option(char name)
{
  for (auto& item : m_items) {
    if (Option* option = dynamic_cast<Option*>(item.get())) {
      if (option->get_short_name() == name) {
        return *option;
      }
    } else if (ShortOptionAlias* alias = dynamic_cast<ShortOptionAlias*>(item.get());
               alias && alias->get_name() == name) {
      return alias->get_option();
    }
  }
  throw std::runtime_error("short option not found");
}

Option&
OptionGroup::lookup_long_option(std::string_view name)
{
  for (auto& item : m_items) {
    if (Option* option = dynamic_cast<Option*>(item.get())) {
      if (option->get_long_name() == name) {
        return *option;
      }
    } else if (LongOptionAlias* alias = dynamic_cast<LongOptionAlias*>(item.get());
               alias && alias->get_name() == name) {
      return alias->get_option();
    }
  }
  throw std::runtime_error("long option not found");
}

} // namespace argparser

/* EOF */
